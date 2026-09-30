#include <TFT_eSPI.h>
#include <math.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite canvas = TFT_eSprite(&tft);

// Screen dimensions
const int SCREEN_W = 135;
const int SCREEN_H = 240;
const int GROUND_Y = 218;

// Length of each session
const unsigned long SPRING_DURATION = 15000;
const unsigned long SUMMER_DURATION = 10000;
const unsigned long AUTUMN_DURATION = 20000;
const unsigned long WINTER_DURATION = 15000;

// One complete year = 60 seconds
const unsigned long YEAR_DURATION = SPRING_DURATION + SUMMER_DURATION + AUTUMN_DURATION + WINTER_DURATION;

// Time between frames
const unsigned long FRAME_INTERVAL = 40;

// Number of objects
const int MAX_BRANCHES = 130;
const int MAX_TIPS = 70;

const int NUM_LEAVES = 200;
const int NUM_SNOWFLAKES = 40;

// Animation parameters
const float TRANSITION = 0.2;
const float HALF = 0.5;
const float THREE_QUARTERS = 0.75;

// Leaf appearance
const float LEAF_SPREAD = 15.0;
const float LEAF_VERTICAL_SPREAD = 0.5;
const float LEAF_MIN_SIZE = 2.0;
const float LEAF_MAX_SIZE = 5.0;
const float LEAF_MAX_ANGLE = 1.0;

// Leaf timing
const float BUD_START = 0.0;
const float BUD_END = 0.75;
const float AUTUMN_START = 0.0;
const float AUTUMN_END = 0.5;
const float FALL_START = 0.4;
const float FALL_END = 0.9;

// Wind
const float WIND_SPEED = 1.0;
const float WIND_MIN = 0.5;
const float WIND_MAX = 1.5;
const float WIND_VERTICAL = 0.5;

// Falling leaves
const float FALL_DRIFT = 15.0;
const float FALL_WAVE_MIN = 2.0;
const float FALL_WAVE_MAX = 5.0;
const float FALL_ROTATION = 2.0;
const float FALL_WAVE_SPEED = 20.0;

// Snow
const float SNOW_MIN_SPEED = 5.0;
const float SNOW_MAX_SPEED = 25.0;
const float SNOW_MIN_DRIFT = 1.0;
const float SNOW_MAX_DRIFT = 5.0;
const float SNOW_MIN_SIZE = 1.0;
const float SNOW_MAX_SIZE = 2.0;

// Stores each branch as two endpoints and a thickness
struct Branch {
  float x1;
  float y1;

  float x2;
  float y2;

  float thickness;
};

// Stores locations where leaves can grow
struct BranchTip {
  float x;
  float y;
};

// Stores the unique properties of each leaf
struct Leaf {
  float baseX;
  float baseY;

  float size;
  float angle;

  float budTime;
  float autumnTime;
  float fallTime;

  float windPhase;
  float windAmount;

  float fallDrift;
  float fallWave;
  float fallRotation;

  int autumnColor;
};

// Stores the properties of each snowflake
struct Snowflake {
  float x;
  float y;

  float speed;
  float drift;
  float phase;

  float size;
};

// Tree, leaf, and snowflake data
Branch branches[MAX_BRANCHES];
BranchTip tips[MAX_TIPS];
Leaf leaves[NUM_LEAVES];
Snowflake snowflakes[NUM_SNOWFLAKES];

int branchCount = 0;
int tipCount = 0;

// Year and frame timing
unsigned long yearStart;
unsigned long previousFrame;

// Seasonal colors
uint16_t SKY_SPRING;
uint16_t SKY_SUMMER;
uint16_t SKY_AUTUMN;
uint16_t SKY_WINTER;

uint16_t GROUND_SPRING;
uint16_t GROUND_SUMMER;
uint16_t GROUND_AUTUMN;
uint16_t GROUND_WINTER;

uint16_t BARK_DARK;
uint16_t BARK_MID;

uint16_t SPRING_BUD;

uint16_t GREEN_DARK;
uint16_t GREEN_MID;
uint16_t GREEN_LIGHT;
uint16_t GREEN_YELLOW;

uint16_t FALL_RED;
uint16_t FALL_ORANGE;
uint16_t FALL_GOLD;
uint16_t FALL_BROWN;

uint16_t SNOW_COLOR;

// Generates a random decimal between a minimum and maximum
float randFloat(float minimum, float maximum) {
  return minimum + ((float)random(0, 10000) / 10000.0) * (maximum - minimum);
}

// Blends between two colors for smooth seasonal transition
uint16_t blendColor(uint16_t color1, uint16_t color2, float amount) {
  amount = constrain(amount, 0.0f, 1.0f);

  uint8_t r1 = ((color1 >> 11) & 0x1F) << 3;
  uint8_t g1 = ((color1 >> 5) & 0x3F) << 2;
  uint8_t b1 = (color1 & 0x1F) << 3;
  uint8_t r2 = ((color2 >> 11) & 0x1F) << 3;
  uint8_t g2 = ((color2 >> 5) & 0x3F) << 2;
  uint8_t b2 = (color2 & 0x1F) << 3;
  uint8_t r = r1 + (r2 - r1) * amount;
  uint8_t b = b1 + (b2 - b1) * amount;
  uint8_t g = g1 + (g2 - g1) * amount;

  return tft.color565(r, g, b);
}

// Adds one branch to the fixed tree
void addBranch(float x1, float y1, float x2, float y2, int thickness) {
  if (branchCount >= MAX_BRANCHES) {
    return;
  }

  branches[branchCount].x1 = x1;
  branches[branchCount].y1 = y1;
  branches[branchCount].x2 = x2;
  branches[branchCount].y2 = y2;
  branches[branchCount].thickness = thickness;

  branchCount++;
}

// Saves a branch tip as a possible location for leaves
void addTip(float x, float y) {
  if (tipCount >= MAX_TIPS) {
    return;
  }

  tips[tipCount].x = x;
  tips[tipCount].y = y;

  tipCount++;
}

// Creates the fixed structure of the tree
void createTree() {
  branchCount = 0;
  tipCount = 0;

  addBranch(67, 216, 67, 184, 7);
  addBranch(67, 184, 66, 158, 6);
  addBranch(66, 158, 68, 136, 5);
  addBranch(68, 136, 67, 116, 4);

  addBranch(67, 181, 54, 164, 5);
  addBranch(54, 164, 42, 148, 4);

  addBranch(42, 148, 29, 137, 3);
  addBranch(29, 137, 19, 124, 2);
  addTip(19, 124);

  addBranch(29, 137, 18, 140, 2);
  addBranch(18, 140, 10, 133, 1);
  addTip(10, 133);

  addBranch(42, 148, 38, 132, 3);
  addBranch(38, 132, 29, 119, 2);
  addTip(29, 119);

  addBranch(38, 132, 41, 116, 2);
  addBranch(41, 116, 36, 104, 1);
  addTip(36, 104);

  addBranch(67, 178, 80, 161, 5);
  addBranch(80, 161, 94, 147, 4);

  addBranch(94, 147, 108, 137, 3);
  addBranch(108, 137, 118, 124, 2);
  addTip(118, 124);

  addBranch(108, 137, 119, 140, 2);
  addBranch(119, 140, 126, 132, 1);
  addTip(126, 132);

  addBranch(94, 147, 98, 130, 3);
  addBranch(98, 130, 108, 117, 2);
  addTip(108, 117);

  addBranch(98, 130, 96, 114, 2);
  addBranch(96, 114, 101, 101, 1);
  addTip(101, 101);

  addBranch(66, 158, 54, 143, 4);
  addBranch(54, 143, 48, 126, 3);

  addBranch(48, 126, 38, 113, 2);
  addBranch(38, 113, 27, 105, 1);
  addTip(27, 105);

  addBranch(38, 113, 35, 98, 1);
  addTip(35, 98);

  addBranch(48, 126, 51, 109, 2);
  addBranch(51, 109, 47, 94, 1);
  addTip(47, 94);

  addBranch(51, 109, 57, 96, 1);
  addTip(57, 96);

  addBranch(67, 154, 79, 139, 4);
  addBranch(79, 139, 85, 122, 3);

  addBranch(85, 122, 95, 109, 2);
  addBranch(95, 109, 106, 100, 1);
  addTip(106, 100);

  addBranch(95, 109, 98, 94, 1);
  addTip(98, 94);

  addBranch(85, 122, 82, 105, 2);
  addBranch(82, 105, 87, 91, 1);
  addTip(87, 91);

  addBranch(82, 105, 76, 93, 1);
  addTip(76, 93);

  addBranch(68, 136, 59, 119, 3);
  addBranch(59, 119, 58, 101, 2);

  addBranch(58, 101, 49, 87, 2);
  addBranch(49, 87, 40, 77, 1);
  addTip(40, 77);

  addBranch(49, 87, 50, 72, 1);
  addTip(50, 72);

  addBranch(58, 101, 64, 86, 2);
  addBranch(64, 86, 61, 70, 1);
  addTip(61, 70);

  addBranch(68, 133, 76, 116, 3);
  addBranch(76, 116, 76, 99, 2);

  addBranch(76, 99, 84, 84, 2);
  addBranch(84, 84, 94, 73, 1);
  addTip(94, 73);

  addBranch(84, 84, 83, 69, 1);
  addTip(83, 69);

  addBranch(76, 99, 69, 84, 2);
  addBranch(69, 84, 72, 68, 1);
  addTip(72, 68);

  addBranch(67, 116, 65, 98, 3);
  addBranch(65, 98, 67, 81, 2);
  addBranch(67, 81, 66, 64, 2);

  addBranch(66, 64, 58, 51, 1);
  addBranch(58, 51, 51, 42, 1);
  addTip(51, 42);

  addBranch(66, 64, 67, 47, 1);
  addBranch(67, 47, 66, 35, 1);
  addTip(66, 35);

  addBranch(66, 64, 75, 51, 1);
  addBranch(75, 51, 82, 41, 1);
  addTip(82, 41);

  addBranch(65, 98, 56, 84, 2);
  addBranch(56, 84, 47, 72, 1);
  addTip(47, 72);

  addBranch(67, 81, 76, 68, 1);
  addBranch(76, 68, 87, 59, 1);
  addTip(87, 59);
}

// Generates a new set of leaves with randomized positions, timing, and movement
void generateLeaves() {
  if(tipCount == 0) {
    return;
  }

  for(int i = 0; i < NUM_LEAVES; i++) {
    // Choose a random branch tip and place the leaf nearby
    int tipIndex = random(0, tipCount);
    BranchTip &tip = tips[tipIndex];

    // Cluster the leaves around branch tips
    float radius = randFloat(0, LEAF_SPREAD);
    float theta = randFloat(0, TWO_PI);
    leaves[i].baseX = tip.x + cos(theta) * radius;
    leaves[i].baseY = tip.y + sin(theta) * radius * LEAF_VERTICAL_SPREAD;

    leaves[i].baseX = constrain(leaves[i].baseX, 5.0f, SCREEN_W - 5.0f);
    leaves[i].baseY = constrain(leaves[i].baseY, 10.0f, GROUND_Y - 30.0f);

    leaves[i].size = randFloat(LEAF_MIN_SIZE, LEAF_MAX_SIZE);
    leaves[i].angle = randFloat(-LEAF_MAX_ANGLE, LEAF_MAX_ANGLE);

    // Give each leaf its own seasonal timing
    leaves[i].budTime = randFloat(BUD_START, BUD_END);

    leaves[i].autumnTime = randFloat(AUTUMN_START, AUTUMN_END);
    leaves[i].fallTime = randFloat(FALL_START, FALL_END);

    // Give each leaf unique wind and falling behavior
    leaves[i].windPhase = randFloat(0, TWO_PI);
    leaves[i].windAmount = randFloat(WIND_MIN, WIND_MAX);

    leaves[i].fallDrift = randFloat(-FALL_DRIFT, FALL_DRIFT);
    leaves[i].fallWave = randFloat(FALL_WAVE_MIN, FALL_WAVE_MAX);
    leaves[i].fallRotation = randFloat(-FALL_ROTATION, FALL_ROTATION);
    leaves[i].autumnColor = random(0, 4);
  }
}

// Generate snowflakes with different starting positions and movement
void generateSnow() {
  for(int i = 0; i < NUM_SNOWFLAKES; i++) {
    snowflakes[i].x = randFloat(0, SCREEN_W);
    snowflakes[i].y = randFloat(-SCREEN_H, GROUND_Y);

    snowflakes[i].speed = randFloat(SNOW_MIN_SPEED, SNOW_MAX_SPEED);
    snowflakes[i].drift = randFloat(SNOW_MIN_DRIFT, SNOW_MAX_DRIFT);
    snowflakes[i].phase = randFloat(0, TWO_PI);
    snowflakes[i].size = randFloat(SNOW_MIN_SIZE, SNOW_MAX_SIZE);
  }
}

// Draw background
void drawBackground(uint16_t sky, uint16_t ground) {
  canvas.fillSprite(sky);

  // Ground
  canvas.fillRect(0, GROUND_Y, SCREEN_W, SCREEN_H - GROUND_Y, ground);
}

// Draw the base of the tree
void drawTrunk() {
  canvas.fillTriangle(56, GROUND_Y, 78, GROUND_Y, 68, 155, BARK_DARK);
  canvas.fillTriangle(62, GROUND_Y, 73, GROUND_Y, 68, 155, BARK_MID);
  canvas.fillTriangle(58, 209, 42, GROUND_Y, 64, GROUND_Y, BARK_DARK);
  canvas.fillTriangle(74, 208, 91, GROUND_Y, 70, GROUND_Y, BARK_DARK);
}

// Draw all fixed branches
void drawBranches() {
  drawTrunk();

  for(int i = 0; i < branchCount; i++) {
    Branch &b = branches[i];
    int thickness = max(1, (int)round(b.thickness));

    uint16_t color;

    if(thickness >= 4) {
      color = BARK_MID;
    } else {
      color = BARK_DARK;
    }

    for(int offset = -thickness / 2; offset <= thickness / 2; offset++) {
      canvas.drawLine(round(b.x1 + offset), round(b.y1), round(b.x2 + offset), round(b.y2), color);
    }
  }
}

 // Draw leaf using two overlapping circles
 void drawLeaf(float x, float y, float size, float angle, uint16_t color) {
  if(x < -5 || x > SCREEN_W + 5 || y < -5 || y > SCREEN_H + 5) {
    return;
  }

  if(size < 1.5) {
    canvas.fillCircle(round(x), round(y), 1, color);
    return;
  }

  float dx = cos(angle) * size;
  float dy = sin(angle) * size;

  int x1 = round(x - dx * 0.5);
  int y1 = round(y - dy * 0.5); 
  int x2 = round(x + dx * 0.5);
  int y2 = round(y + dy * 0.5);
  int radius = max(1, (int)(size * 0.5));

  canvas.fillCircle(x1, y1, radius, color);
  canvas.fillCircle(x2, y2, radius, color);

  if (size >= 3.0) {
    canvas.drawLine(round(x - dx * 0.5), round(y - dy * 0.5), round(x + dx * 0.5), round(y + dy * 0.5), GREEN_DARK);
  }
 }

// Color
uint16_t getGreenColor(int index) {
  switch(index % 4) {
    case 0: return GREEN_DARK;
    case 1: return GREEN_MID;
    case 2: return GREEN_LIGHT;
    default: return GREEN_YELLOW;
  }
}

uint16_t getAutumnColor(int index) {
  switch(index % 4) {
    case 0: return FALL_RED;
    case 1: return FALL_ORANGE;
    case 2: return FALL_GOLD;
    default: return FALL_BROWN;
  }
}

// Leaves gradually bud, grow, and turn green
void drawSpring(float progress) {
  uint16_t sky = blendColor(SKY_SPRING, SKY_SUMMER, progress * 0.25); 
  drawBackground(sky, GROUND_SPRING);
  drawBranches();
  float time = millis() * 0.001;

  for(int i = 0; i < NUM_LEAVES; i++) {
    Leaf &leaf = leaves[i];

    // Start each leaf at its individual budding time
    if(progress < leaf.budTime) {
      continue;
    }

    // Gradually grow the bud to full size
    float growth = (progress - leaf.budTime) / TRANSITION;
    growth = constrain(growth, 0.0f, 1.0f);

    growth = growth * growth * (3.0 - 2.0 * growth);

    float size = leaf.size * growth;

    uint16_t color = blendColor(SPRING_BUD, getGreenColor(i), growth);

    float sway = sin(time * WIND_SPEED + leaf.windPhase) * leaf.windAmount * WIND_VERTICAL;

    drawLeaf(leaf.baseX + sway, leaf.baseY, size, leaf.angle, color);
  }
}

// Leaves move gently in the wind
void drawSummer(float progress) {
  uint16_t sky = blendColor(SKY_SPRING, SKY_SUMMER, 0.75 + progress * 0.25);
  drawBackground(sky, GROUND_SUMMER);
  drawBranches();
  float time = millis() * 0.001;

  for(int i = 0; i < NUM_LEAVES; i++) {
    Leaf &leaf = leaves[i];

    float swayX = sin(time * WIND_SPEED + leaf.windPhase) * leaf.windAmount; 
    float swayY = cos(time * WIND_SPEED + leaf.windPhase) * WIND_VERTICAL;
    drawLeaf(leaf.baseX + swayX, leaf.baseY + swayY, leaf.size, leaf.angle, getGreenColor(i));
  }
}

// Leaves change color and fall
void drawAutumn(float progress) {
  uint16_t sky = blendColor(SKY_SUMMER, SKY_AUTUMN, constrain(progress / HALF, 0.0f, 1.0f));
  uint16_t ground = blendColor(GROUND_SUMMER, GROUND_AUTUMN, progress);
  drawBackground(sky, ground);
  drawBranches();
  float time = millis() * 0.001;

  for(int i = 0; i < NUM_LEAVES; i++){
    Leaf &leaf = leaves[i];

    uint16_t green = getGreenColor(i);
    uint16_t autumn = getAutumnColor(leaf.autumnColor);
    float colorProgress = (progress - leaf.autumnTime) / TRANSITION;

    colorProgress = constrain(colorProgress, 0.0f, 1.0f);

    uint16_t currentColor = blendColor(green, autumn, colorProgress);

    if(progress < leaf.fallTime) {
      float wind = sin(time * WIND_SPEED + leaf.windPhase) * leaf.windAmount * (1.0 + progress);
      drawLeaf(leaf.baseX + wind, leaf.baseY, leaf.size, leaf.angle, currentColor);
    } else {
      float fallProgress = (progress - leaf.fallTime) / (1.0 - leaf.fallTime);
      fallProgress = constrain(fallProgress, 0.0f, 1.0f);

      // Simulate falling with gravity, horizontal shift, and side-to-side movement
      float gravity = fallProgress * fallProgress;
      float y = leaf.baseY + gravity * (GROUND_Y - leaf.baseY + 5);
      float x = leaf.baseX + leaf.fallDrift * fallProgress;
      x += sin(fallProgress * FALL_WAVE_SPEED + leaf.windPhase) * leaf.fallWave;
      float rotation = leaf.angle + fallProgress * leaf.fallRotation;

      if(y < GROUND_Y + 2){
        drawLeaf(x, y, leaf.size, rotation, currentColor);
      }
    }
  }
}

// Falling snow
void updateDrawSnow(float progress, float deltaTime) {
  float intensity;

  // Snow builds at the beginning of winter and melts near the end
  if(progress < TRANSITION) {
    intensity = progress / TRANSITION;
  } else if(progress < THREE_QUARTERS) {
    intensity = 1.0;
  } else {
    intensity = (1.0 - progress) / (1.0 - THREE_QUARTERS);
  }

  intensity = constrain(intensity, 0.0f, 1.0f);

  float time = millis() * 0.001;

  for(int i = 0; i < NUM_SNOWFLAKES; i++){
    Snowflake &snow = snowflakes[i];
    snow.y += snow.speed * deltaTime;
    snow.x += sin(time + snow.phase) * snow.drift * deltaTime;

    // Recycle snowflakes that reach the ground
    if(snow.y > GROUND_Y) {
      snow.y = randFloat(-25, -5);
      snow.x = randFloat(0, SCREEN_W);
    }
    
    if(snow.x < -3) {
      snow.x = SCREEN_W + 2;
    }

    if(snow.x > SCREEN_W + 3){
      snow.x = -2;
    }

    float visibility = randFloat(0, 1);

    if(visibility < intensity) {
      int radius = max(1, (int)round(snow.size));
      canvas.fillCircle(round(snow.x), round(snow.y), radius, SNOW_COLOR);
    }
  }
}

// Builds up snow and melts it
void drawSnowAccumulation(float progress) {
  float accumulation;
  
  if(progress < TRANSITION) {
    accumulation = progress / TRANSITION;
  } else if(progress < THREE_QUARTERS) {
    accumulation = 1.0;
  } else {
    accumulation = (1.0 - progress) / (1.0 - THREE_QUARTERS);
  }

  accumulation = constrain(accumulation, 0.0f, 1.0f);

  if(accumulation > 0.2) {
    int snowHeight = 2 + accumulation * 5;
    canvas.fillRect(0, GROUND_Y - snowHeight / 2, SCREEN_W, snowHeight, SNOW_COLOR);
  }

  if(accumulation > 0.4) {
    for(int i = 0; i < branchCount; i += 5) {
      Branch &b = branches[i];

      float verticalDifference = abs(b.y2 - b.y1);
      float horizontalDifference = abs(b.x2 - b.x1);

      if(horizontalDifference > verticalDifference * HALF) {
        float snowX = (b.x1 + b.x2) / 2.0;
        float snowY = min(b.y1, b.y2) - 1;
        canvas.fillCircle(round(snowX), round(snowY), 1, SNOW_COLOR);
      }
    }
  }
}

// Snow falls and back to spring
void drawWinter(float progress, float deltaTime) {
  uint16_t sky;
  uint16_t ground;

  if(progress < THREE_QUARTERS) {
    sky = blendColor(SKY_AUTUMN, SKY_WINTER, constrain(progress / HALF, 0.0f, 1.0f));
    ground = GROUND_WINTER;
  } else {
    float springTransition = (progress - THREE_QUARTERS) / (1.0 - THREE_QUARTERS);

    springTransition = springTransition * springTransition * (3.0 - 2.0 * springTransition);
    sky = blendColor(SKY_WINTER, SKY_SPRING, springTransition);
    ground = blendColor(GROUND_WINTER, GROUND_SPRING, springTransition);
  }

  drawBackground(sky, ground);
  drawBranches();
  drawSnowAccumulation(progress);
  updateDrawSnow(progress, deltaTime);
}

// Start a new yearly cycle with new leaves
void startNewYear() {
  generateLeaves();
  generateSnow();
  yearStart = millis();
}

void setupColors() {
  SKY_SPRING = tft.color565(188, 220, 229);
  SKY_SUMMER = tft.color565(115, 184, 218);
  SKY_AUTUMN = tft.color565(191, 191, 177);
  SKY_WINTER = tft.color565(195, 205, 214);

  GROUND_SPRING = tft.color565(95, 137, 73);
  GROUND_SUMMER = tft.color565(65, 112, 54);
  GROUND_AUTUMN = tft.color565(132, 105, 65);
  GROUND_WINTER = tft.color565(218, 224, 228);

  BARK_DARK = tft.color565(65, 46, 35);
  BARK_MID = tft.color565(91, 64, 43);

  SPRING_BUD = tft.color565(145, 184, 82);

  GREEN_DARK = tft.color565(42, 89, 45);
  GREEN_MID = tft.color565(57, 116, 52);
  GREEN_LIGHT = tft.color565(76, 139, 61);
  GREEN_YELLOW = tft.color565(101, 151, 66);

  FALL_RED = tft.color565(164, 62, 39);
  FALL_ORANGE = tft.color565(207, 99, 34);
  FALL_GOLD = tft.color565(218, 151, 42);
  FALL_BROWN = tft.color565(135, 75, 39);

  SNOW_COLOR = tft.color565(242, 245, 247);
}

void setup() {
  Serial.begin(115200);
  tft.init();

  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  canvas.setColorDepth(16);
  canvas.createSprite(SCREEN_W, SCREEN_H);

  randomSeed(micros());
  
  setupColors();

  createTree();

  generateLeaves();
  generateSnow();

  yearStart = millis();
  previousFrame = millis();
}

void loop(){
  unsigned long now = millis();

  // Limit how often a new frame is drawn
  if(now - previousFrame < FRAME_INTERVAL) {
    return;
  }

  // Calculate time sicne the previous frame in seconds
  float deltaTime = (now - previousFrame) / 1000.0;
  previousFrame = now;

  // Track progress through the current yearly cycle
  unsigned long elapsed = now - yearStart;

  if(elapsed >= YEAR_DURATION) {
    startNewYear();
    elapsed = 0;
  }

  // Determine the current season and its progress
  if(elapsed < SPRING_DURATION) {
    float progress = (float)elapsed / SPRING_DURATION;
    drawSpring(progress);

  } else if(elapsed < SPRING_DURATION + SUMMER_DURATION) {
    unsigned long seasonElapsed = elapsed - SPRING_DURATION;
    float progress = (float)seasonElapsed / SUMMER_DURATION;
    drawSummer(progress);

  } else if(elapsed < SPRING_DURATION + SUMMER_DURATION + AUTUMN_DURATION) {
    unsigned long seasonElapsed = elapsed - SPRING_DURATION - SUMMER_DURATION;
    float progress = (float)seasonElapsed / AUTUMN_DURATION;
    drawAutumn(progress);

  } else {
    unsigned long seasonElapsed = elapsed - SPRING_DURATION - SUMMER_DURATION - AUTUMN_DURATION;
    float progress = (float)seasonElapsed / WINTER_DURATION;
    drawWinter(progress, deltaTime);
    
  }

  canvas.pushSprite(0, 0);
}