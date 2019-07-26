#include <SFML/Graphics.hpp>

#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

const int WIDTH = 800; //MAX = 8192
const int HEIGHT = 800; //MAX = 8192
int K_MAX = 20;
sf::Vertex mandel[HEIGHT * WIDTH];

struct Position
{
    double x, y;
};

Position real_origin = {0, 0};
const int pixel_size = 1;
double lambda = 150; //lambda pixels = 1 unité //2 unités de hauteur c'est bien pour tout voir


std::string get_file_name()
{
    double realW = WIDTH / lambda;
    double realH = HEIGHT / lambda;

    std::vector<double> fnames = {WIDTH, HEIGHT, realW, realH, real_origin.x, real_origin.y, K_MAX};
    std::vector<std::string> snames;
    for(double d : fnames)
    {
        std::stringstream ss;
        ss << std::setprecision(4);
        ss << d;
        snames.push_back(ss.str());
    }

    return "mandel_" + snames[0] + "x" + snames[1] + "_" + snames[2] + "x" + snames[3]
    + "_(Ox,Oy)(" + snames[4] + "," + snames[5] + ")" + "_KMAX" + snames[6] + ".jpg";
}

sf::Image get_screenshot()
{
    sf::RenderTexture screen;
    screen.create(WIDTH, HEIGHT);
    screen.draw(mandel, WIDTH * HEIGHT, sf::Points);
    screen.display();

    sf::Texture boudi = screen.getTexture();

    return boudi.copyToImage();
}

sf::Vector2f coord_transfo(sf::Vector2f a)
{
    double X = (a.x - (double)(WIDTH / 2)) / lambda + real_origin.x;
    double Y = ((double)(HEIGHT / 2) - a.y) / lambda + real_origin.y;
    return sf::Vector2f(X, Y);
}

double module(sf::Vector2f a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

int calc_k(sf::Vector2f c)
{
    sf::Vector2f z = c;
    int k = 1;
    while(module(z) <= 2 && k < K_MAX)
    {
        sf::Vector2f new_z;
        new_z.x = z.x * z.x - z.y * z.y + c.x;
        new_z.y = 2.0 * z.x * z.y + c.y;
        z = new_z;
        k++;
    }

    return k;
}

double calc_intensity(int k)
{
    return 255.0 - (pow(- (double)k / K_MAX + 1, -0.1) - 1) * 255.0;
}

void color_pixel(int x, int y, int intensity)
{
    for(int i = y; i < y + pixel_size; i++)
    {
        for(int j = x; j < x + pixel_size; j++)
        {
            mandel[i * WIDTH + j].position = sf::Vector2f(j,i);
            mandel[i * WIDTH + j].color = sf::Color(intensity, intensity, intensity, intensity);
        }
    }
}
void compute_mandelbrot()
{
    for(int i = 0; i < HEIGHT; i += pixel_size)
    {
        for(int j = 0; j < WIDTH; j += pixel_size)
        {
            sf::Vector2f c = coord_transfo(sf::Vector2f(j,i));

            int k = calc_k(c);
            int intensity = calc_intensity(k);

            color_pixel(j,i,intensity);
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "M");

    bool is_dragging = false;
    Position old_pos;
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;

        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::MouseWheelScrolled)
            {
                lambda *= pow(2,event.mouseWheelScroll.delta);
                K_MAX *= pow(1.5,event.mouseWheelScroll.delta);

            }
            else if(event.type == sf::Event::MouseButtonPressed)
            {
                is_dragging = true;
                old_pos = {event.mouseButton.x, event.mouseButton.y};
            }
            else if(event.type == sf::Event::MouseButtonReleased)
                is_dragging = false;
            else if(event.type == sf::Event::MouseMoved && is_dragging)
            {
                real_origin.x += (old_pos.x - event.mouseMove.x) / lambda;
                real_origin.y += (event.mouseMove.y - old_pos.y) / lambda;
                old_pos.x = event.mouseMove.x;
                old_pos.y = event.mouseMove.y;
            }
            else if (event.type == sf::Event::Closed)
                window.close();
        }

        //std::cout << K_MAX << '\n';
        window.clear(sf::Color::Black);
        compute_mandelbrot();
        window.draw(mandel, HEIGHT * WIDTH, sf::Points);
        window.display();
    }



    sf::Image screenshot = get_screenshot();

    std::string name = get_file_name();
    screenshot.saveToFile(name);

    return 0;
}
