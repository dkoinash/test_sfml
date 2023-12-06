#include <cstdint>
#include <mutex>
#include <random> // std::uniform_int_distribution
#include <thread>
#include <vector>

#include <SFML/Graphics.hpp>

#ifdef WIN32
#include <windows.h>

#define ENTRYPOINT_MAIN                                                                            \
  int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow)
#else
#define ENTRYPOINT_MAIN int main(int argc, char* argv[])
#endif

static const uint32_t WIN_WIDTH  = 1920;
static const uint32_t WIN_HEIGHT = 1008;
static const uint32_t FPS_LIMIT  = 60;

static const sf::Color BACKGROUND_CLR = sf::Color(1, 1, 1);

struct Point
{
  Point()
  {
    x = 0;
    y = 0;
  }
  Point(uint32_t x_, uint32_t y_)
    : x(x_)
    , y(y_)
  {}
  void setColor(uint8_t _red = 0, uint8_t _green = 0, uint8_t _blue = 0)
  {
    red   = _red;
    green = _green;
    blue  = _blue;
  }

  uint32_t x = 0;
  uint32_t y = 0;
  uint8_t  red;
  uint8_t  green;
  uint8_t  blue;
};

std::vector<Point> field;
std::mutex         lockField;

auto
randomizer(int minValue, int maxValue)
{
  std::random_device              rd;
  std::mt19937                    gen(rd());
  std::uniform_int_distribution<> distrib(minValue, maxValue);

  return distrib(gen);
}

void
changeColor()
{
  int pos = 0;
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
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
fillingField()
{
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    lockField.lock();

    field.push_back(Point(randomizer(0, WIN_WIDTH), randomizer(0, WIN_HEIGHT)));

    lockField.unlock();
  }
}

ENTRYPOINT_MAIN
{
  sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "SFML works!", sf::Style::None);
  window.setFramerateLimit(FPS_LIMIT);

  sf::Font font;
  if (!font.loadFromFile("Poiret One.ttf")) return false;

  std::thread th_field(fillingField);
  std::thread th_color(changeColor);
  th_field.detach();
  th_color.detach();

  while (window.isOpen()) {
    sf::Event event;
#if DEPRECATED

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto rndWidth = []() {
      std::random_device              rd;
      std::mt19937                    gen(rd());
      std::uniform_int_distribution<> distrib(0, WIN_WIDTH);

      uint32_t value;

      value = distrib(gen);

      return value;
    };

    auto rndHeight = []() {
      std::random_device              rd;
      std::mt19937                    gen(rd());
      std::uniform_int_distribution<> distrib(0, WIN_HEIGHT);

      uint32_t value;

      value = distrib(gen);

      return value;
    };

    // Point point;
    // point.x = rndWidth();
    // point.y = rndHeight();

    field.push_back(Point(rndWidth(), rndHeight()));
#else

#endif // DEPRECATED

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
      sf::CircleShape circle(1.0f);
      lockField.lock();
      circle.move(field[i].x, field[i].y);

      circle.setFillColor(sf::Color(field[i].red, field[i].green, field[i].blue));
      lockField.unlock();
      window.draw(circle);
    }

    window.display();
  }

  return 0;
}