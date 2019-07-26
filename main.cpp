#include <SFML/Graphics.hpp>

#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

const int WIDTH = 3000; //MAX = 8192
const int HEIGHT = 3000; //MAX = 8192
const int K_MAX = 100;
sf::Vertex mandel[HEIGHT * WIDTH];

const double centerX = -0.5; //
const double centerY = 0;
const double lambda = 1500; //lambda pixels = 1 unité //2 unités de hauteur c'est bien pour tout voir


std::string get_file_name()
{
    double realW = WIDTH / lambda;
    double realH = HEIGHT / lambda;

    std::vector<double> fnames = {WIDTH, HEIGHT, realW, realH, centerX, centerY, K_MAX};
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

sf::Vector2f coord_transfo(sf::Vector2f a)
{


    double X = (a.x - (double)(WIDTH / 2)) / lambda + centerX;
    double Y = ((double)(HEIGHT / 2) - a.y) / lambda + centerY;
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
    return 255.0 - (double)k / K_MAX * 255.0;
}
int main()
{
   // sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Fullscreen);


    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = 0; j < WIDTH; j++)
        {
            mandel[i * WIDTH + j].position = sf::Vector2f(j,i);
            sf::Vector2f c = coord_transfo(sf::Vector2f(j,i));

            int k = calc_k(c);
            int intensity = calc_intensity(k);

            mandel[i * WIDTH + j].color = sf::Color(intensity, intensity, intensity, intensity);
        }
    }

  //  window.draw(mandel, HEIGHT * WIDTH, sf::Points);
    sf::RenderTexture screen;
    screen.create(WIDTH, HEIGHT);
    screen.draw(mandel, WIDTH * HEIGHT, sf::Points);
    screen.display();

    sf::Texture boudi = screen.getTexture();

    sf::Image screenshot = boudi.copyToImage();

    std::string name = get_file_name();
    screenshot.saveToFile(name);


    return 0;
}
