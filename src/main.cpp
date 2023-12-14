#include <cmath>
#include <cstdint>
#include <mutex>
#include <random> // std::uniform_int_distribution
#include <thread>
#include <vector>

#define PI std::acos(-1)

#include <SFML/Graphics.hpp>

#ifdef WIN32
#include <windows.h>

#define ENTRYPOINT_MAIN                                                                            \
  int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow)
#else
#define ENTRYPOINT_MAIN int main(int argc, char* argv[])
#endif

static const uint32_t WIN_WIDTH     = 1920;
static const uint32_t WIN_HEIGHT    = 1080;
static const uint32_t FPS_LIMIT     = 60;
static const uint32_t MAX_DOT_COUNT = 800;
static const float    STEP_ANGLE    = 0.02f;
static const float    MIN_SIZE      = 0.6f;
static const float    MAX_SIZE      = 1.8f;

static const sf::Color BACKGROUND_CLR = sf::Color(1, 1, 1);

struct Point
{
  Point()
  {
    x     = 0.f;
    y     = 0.f;
    r     = 0.f;
    red   = 255;
    green = 255;
    blue  = 255;
  }
  Point(float   x_,
        float   y_,
        float   r_     = 1.f,
        uint8_t red_   = 255,
        uint8_t green_ = 255,
        uint8_t blue_  = 255)
    : x(x_)
    , y(y_)
    , r(r_)
    , red(red_)
    , green(green_)
    , blue(blue_)
  {}
  void setColor(uint8_t _red, uint8_t _green, uint8_t _blue)
  {
    red   = _red;
    green = _green;
    blue  = _blue;
  }

  float   x;
  float   y;
  float   r;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

std::vector<Point> field;
std::mutex         lockField;

template<class T>
T
randomizer(T minValue, T maxValue)
{
  std::random_device               rd;
  std::mt19937                     gen(rd());
  std::uniform_real_distribution<> distrib(minValue, maxValue);

  return distrib(gen);
}

void
changeColor()
{
  int pos = 0;
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lockField.lock();
    field[pos].setColor(randomizer(0, 255), randomizer(0, 255), randomizer(0, 255));
    lockField.unlock();
    if (pos >= field.size())
      pos = 0;
    else
      ++pos;
  }
}

void
rotate(Point base = Point(0.f, 0.f), float angle = 0.f)
{
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    float radAngle = angle / 180 * PI;

    lockField.lock();
    for (auto it = field.begin(); it != field.end(); ++it) {
      Point pnt = *it;

      float loc_x = pnt.x - base.x;
      float loc_y = pnt.y - base.y;
      float r     = std::sqrtf(std::powf(loc_x, 2.f) + std::powf(loc_y, 2.f));
      float alfa  = std::acosf(loc_x / r);
      pnt.x       = r * std::cosf(alfa + radAngle);
      pnt.y       = std::sqrtf(std::powf(r, 2.f) - std::powf(pnt.x, 2.f));
      pnt.x       = pnt.x + base.x;
      pnt.y       = pnt.y + base.y;

      *it = pnt;
    }
    lockField.unlock();
  }
}

void
fillingField()
{
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    lockField.lock();

    field.push_back(Point(randomizer(-float(WIN_WIDTH), float(WIN_WIDTH)),
                          randomizer(-float(WIN_HEIGHT), float(WIN_HEIGHT)),
                          randomizer(MIN_SIZE, MAX_SIZE)));
    if (field.size() > MAX_DOT_COUNT) field.erase(field.begin());

    lockField.unlock();
  }
}

ENTRYPOINT_MAIN
{
  sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT),
                          "SFML works!",
                          sf::Style::Fullscreen); // sf::Style::Close); // sf::Style::None
  window.setFramerateLimit(FPS_LIMIT);

  sf::Font font;
  if (!font.loadFromFile("Poiret One.ttf")) return false;

  std::thread th_field(fillingField);
  std::thread th_color(changeColor);
  Point       base;//(WIN_WIDTH / 2, WIN_HEIGHT / 2);
  std::thread th_rotation(rotate, base, STEP_ANGLE);
  th_field.detach();
  th_color.detach();
  th_rotation.detach();

  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        return 0;
      }

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::Escape) {
          window.close();
          return 0;
        }
      }
    }

    window.clear(BACKGROUND_CLR);

    for (int i = 0; i < field.size(); ++i) {
      lockField.lock();
      sf::CircleShape circle(field[i].r);
      if (field[i].x > 0.f && field[i].y > 0.f && field[i].x < float(WIN_WIDTH) &&
          field[i].y < float(WIN_HEIGHT))
        circle.move(field[i].x, field[i].y);

      circle.setFillColor(sf::Color(field[i].red, field[i].green, field[i].blue));
      lockField.unlock();
      window.draw(circle);
    }

    window.display();
  }

  return 0;
}