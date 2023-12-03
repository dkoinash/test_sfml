#include <cstdint>
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
static const sf::Color GRID_COLOR     = sf::Color(255, 255, 255, 64);

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

  uint32_t x = 0;
  uint32_t y = 0;
};

std::vector<Point> field;

ENTRYPOINT_MAIN
{
  sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "SFML works!", sf::Style::None);
  window.setFramerateLimit(FPS_LIMIT);

  sf::Font font;
  if (!font.loadFromFile("Poiret One.ttf")) return false;

  while (window.isOpen()) {
    sf::Event event;

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
      circle.move(field[i].x, field[i].y);
      circle.setFillColor(sf::Color(128, 128, 128));
      window.draw(circle);
    }

    window.display();
  }

  return 0;
}