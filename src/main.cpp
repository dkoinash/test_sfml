#include <cmath>
#include <cstdint>
#include <mutex>
#include <random> // std::uniform_int_distribution
#include <thread>
#include <vector>

#include <struct.h>

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
static const uint32_t STEP_TIME     = 1; // мс
static const float    BALL_RADIUS   = 3.99;
static const float    STEP_ANGLE    = 0.02f;
static const float    MIN_SIZE      = 0.6f;
static const float    MAX_SIZE      = 1.8f;

Point      m_point;
std::mutex lockPoint;

void
fallenPoint()
{
  bool   isFall = true;
  double timer  = 0;
  while (true) {
    if (isFall) { // Падение
      lockPoint.lock();
      m_point.y += m_point.y * std::powf(timer, 2) / 2.f;
      if (m_point.y >= WIN_HEIGHT - BALL_RADIUS) {
        isFall = false;
        timer  = 0;
      };
      lockPoint.unlock();
    } else { // Отскок
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      isFall    = true;
      m_point.x = WIN_WIDTH / 2;
      m_point.y = 200;
      // timer  = 0;
    }
    timer += 0.001 * STEP_TIME;
    std::this_thread::sleep_for(std::chrono::milliseconds(STEP_TIME));
  }
}

static const sf::Color BACKGROUND_CLR = sf::Color(1, 1, 1);

ENTRYPOINT_MAIN
{
  sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT),
                          "SFML works!",
                          sf::Style::Fullscreen); // sf::Style::Close); // sf::Style::None
  window.setFramerateLimit(FPS_LIMIT);

  sf::Font font;
  if (!font.loadFromFile("Poiret One.ttf")) return false;

  // std::thread th_field(fillingField);
  // std::thread th_color(changeColor);
  // Point       base;//(WIN_WIDTH / 2, WIN_HEIGHT / 2);
  // std::thread th_rotation(rotate, base, STEP_ANGLE);
  // th_field.detach();
  // th_color.detach();
  // th_rotation.detach();

  m_point.x = WIN_WIDTH / 2;
  m_point.y = 200;

  std::thread th_fallenPoint(fallenPoint);
  th_fallenPoint.detach();

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

    sf::CircleShape circle(BALL_RADIUS);
    lockPoint.lock();
    circle.move(m_point.x, m_point.y);
    lockPoint.unlock();
    circle.setFillColor(sf::Color(255, 255, 255));
    window.draw(circle);
    // for (int i = 0; i < field.size(); ++i) {
    //   lockField.lock();
    //   sf::CircleShape circle(field[i].r);
    //   if (field[i].x > 0.f && field[i].y > 0.f && field[i].x < float(WIN_WIDTH) &&
    //       field[i].y < float(WIN_HEIGHT))
    //     circle.move(field[i].x, field[i].y);

    //   circle.setFillColor(sf::Color(field[i].red, field[i].green, field[i].blue));
    //   lockField.unlock();
    //   window.draw(circle);
    // }

    window.display();
  }

  return 0;
}