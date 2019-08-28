#include <SFML/Graphics.hpp>

#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <complex>


const int WIDTH = 2000; //MAX = 8192
const int HEIGHT = 2000; //MAX = 8192
int K_MAX = 50;
sf::Vertex mandel[HEIGHT * WIDTH];

struct Position
{
    double x, y;
};

Position real_origin = {-0.5,0};
double pixels_per_unit = 1000; //pixels_per_unit pixels = 1 uniy //2 height units are enough to see the whole thing


std::string get_file_name()
{
    double realW = WIDTH / pixels_per_unit;
    double realH = HEIGHT / pixels_per_unit;

    std::vector<double> fnames = {WIDTH, HEIGHT, realW, realH, real_origin.x, real_origin.y, K_MAX};
    std::vector<std::string> snames;
    for(double d : fnames)
    {
        std::stringstream ss;
        ss << std::setprecision(7);
        ss << d;
        snames.push_back(ss.str());
    }

    return "./pictures/mandel_" + snames[0] + "x" + snames[1] + "_" + snames[2] + "x" + snames[3]
    + "_(Ox,Oy)(" + snames[4] + "," + snames[5] + ")" + "_KMAX" + snames[6] + "boudi.jpg";
}



std::complex<double> coord_transfo(Position p)
{
    double X = (p.x - (double)(WIDTH / 2)) / pixels_per_unit + real_origin.x;
    double Y = ((double)(HEIGHT / 2) - p.y) / pixels_per_unit + real_origin.y;
    return {X, Y};
}

bool is_inside_safe_area(std::complex<double> c)
{
    double x = c.real(), y = c.imag();

    return (x < sqrt(y * y + pow(x - 1.0 / 4, 2)) - 2 * y * y - 2 * pow(x - 1.0 / 4, 2) + 1.0 / 4)
    || ((x + 1) * (x + 1) + y * y < 1.0 / 16);
}

int calc_k(std::complex<double> c)
{
    if(is_inside_safe_area(c))
        return K_MAX;

    std::complex<double> z = c;
    int k = 1;
    while(std::abs(z) <= 2 && k < K_MAX)
    {
        z = z * z + c;
        k++;
    }
    return k;
}

sf::Color get_color(double t)
{
    //return 255.0 - (double)k / K_MAX * 255.0;
    //return 255.0 - (pow( -(double)k / K_MAX + 1, -0.1) - 1) * 255.0;
    //return 255.0 - (k == K_MAX) * 255.0;
    //return 255.0 - pow(2, 100 * ((double)k / K_MAX)) * 255.0;

    int r = (int)(9*(1-t)*t*t*t*255);
    int g = (int)(15*(1-t)*(1-t)*t*t*255);
    int b =  (int)(8.5*(1-t)*(1-t)*(1-t)*t*255);

    return sf::Color(r,g,b,255);
}

void compute_mandelbrot()
{
    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = 0; j < WIDTH; j++)
        {
            std::complex<double> c = coord_transfo({j,i});

            int k = calc_k(c);

            mandel[i * WIDTH + j].position = sf::Vector2f(j,i);
            mandel[i * WIDTH + j].color = get_color((double)(k / K_MAX));
        }
    }
}

void take_screenshot()
{
    sf::RenderTexture screen;
    screen.create(WIDTH, HEIGHT);
    compute_mandelbrot();
    screen.draw(mandel, WIDTH * HEIGHT, sf::Points);
    screen.display();

    sf::Texture boudi = screen.getTexture();

    sf::Image screenshot = boudi.copyToImage();
    std::string name = get_file_name();
    screenshot.saveToFile(name);
}

void launch_window()
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
            if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::F10)
                    take_screenshot();
            }
            else if(event.type == sf::Event::MouseWheelScrolled)
            {
                pixels_per_unit *= pow(2,event.mouseWheelScroll.delta);
                //K_MAX *= pow(1.5,event.mouseWheelScroll.delta);
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
                real_origin.x += (old_pos.x - event.mouseMove.x) / pixels_per_unit;
                real_origin.y += (event.mouseMove.y - old_pos.y) / pixels_per_unit;
                old_pos.x = event.mouseMove.x;
                old_pos.y = event.mouseMove.y;

            }
            else if (event.type == sf::Event::Closed)
                window.close();
        }

        compute_mandelbrot();
        window.clear(sf::Color::Black);
        window.draw(mandel, HEIGHT * WIDTH, sf::Points);
        window.display();
        std::cout << real_origin.x << " "  << real_origin.y << '\n';
        //std::cout << K_MAX << '\n';


    }
}


int main()
{
   // launch_window();

    take_screenshot();

    return 0;
}
