#include "turtle.h"
#include "turtle.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ITERATIONS 100000
#define MAX_ANTS 100
#define WIDTH 500
#define HEIGHT 500

struct ant
{
    int x_pos;              // pozycja na osi x
    int y_pos;              // pozycja na osi y
    char current_direction; // aktualny kierunek mrowki
    int rgb_r;
    int rgb_g;
    int rgb_b;
    bool moving; // 1 - mrowka sie porusza, 0 - mrowka stoi
};

int area[WIDTH][HEIGHT] = {0}; // definiowana jest tablica 2-wymiarowa, przechowująca stan zamalowania pikseli na planszy

// czytanie konfiguracji
void readConfigurations(int *iter, int *ants_count);

// zmiana kierunku ruchu mrowki w lewo
void turn_left(struct ant *ptr);

// zmiana kierunku ruchu mrowki w prawo
void turn_right(struct ant *ptr);

// krok do przeodu w zaleznosci od kierunku ruchu
void go_forward(struct ant *ptr);

// zatrzymanie danej mrowki, jezeli przekroczy granice pola
void check_ant_stop(struct ant *ptr);

// generowanie randomowego numeru w przedziale min_int do max_int
int generate_random_int(int min_int, int max_int);

// ustawianie poczatkowych parametrow mrowki
void ant_init(struct ant *ptr);

// wykonywanie ruchu przez mrowke
void ant_walk(struct ant *ptr);

int main()
{
    srand(time(NULL));

    struct ant *ant_ptr;
    int i, j, ants_count, iter;

    readConfigurations(&iter, &ants_count);

    printf("Liczba iteracji = %d\n", iter);
    printf("Ilosc mrowek = %d\n", ants_count);

    turtle_init(WIDTH, HEIGHT); //Inicjalizacja planszy

    if (iter > MAX_ITERATIONS)
    {
        printf("liczba iteracji musi byc mniejsza lub rowna %d", MAX_ITERATIONS);
        return 1;
    };
    if (ants_count > MAX_ANTS)
    {
        printf("liczba mrowek musi byc mniejsza lub rowna %d", MAX_ANTS);
        return 2;
    };

    // alokacja pamieci dla zadanej ilosci mrowek
    ant_ptr = (struct ant *)malloc(ants_count * sizeof(struct ant));

    for (i = 0; i < ants_count; ++i)
    // nadanie parametrów każdej z mrówek
    {
        ant_init(ant_ptr + i);
    }

    for (j = 0; j < iter; j++)
    {
        for (i = 0; i < ants_count; i++)
        {
            check_ant_stop(ant_ptr + i);
            ant_walk(ant_ptr + i);
        }
    }

    turtle_save_bmp("output.bmp"); // zapisanie wyniku wyjściowego do pliku
    return 0;
}

void readConfigurations(int *iter, int *ants_count)
{
    FILE *config;
    config = fopen("config.txt", "r");
    if (config != NULL)
    {
        fscanf(config, "iter=%d\n", iter);
        fscanf(config, "ants_count=%d\n", ants_count);
        printf("Odczyt pliku przebiegl pomyslnie!\n");
        fclose(config);
    }
    else
    {
        printf("Odczyt pliku nie powiodl sie, parametry zostana ustawione na domyslne\n");
        *iter = 10000;
        *ants_count = 6;
    }
}

void turn_left(struct ant *ptr)
{
    // Kierunek ruchu mrowki jest zmieniany w zaleznosci od obecnego kierunku

    switch (ptr->current_direction)
    {
    case 'N':
        ptr->current_direction = 'W';
        break;
    case 'W':
        ptr->current_direction = 'S';
        break;
    case 'S':
        ptr->current_direction = 'E';
        break;
    case 'E':
        ptr->current_direction = 'N';
        break;
    }
};

void turn_right(struct ant *ptr)
{
    // Kierunek ruchu mrowki jest zmieniany w zaleznosci od obecnego kierunku
    switch (ptr->current_direction)
    {
    case 'N':
        ptr->current_direction = 'E';
        break;
    case 'E':
        ptr->current_direction = 'S';
        break;
    case 'S':
        ptr->current_direction = 'W';
        break;
    case 'W':
        ptr->current_direction = 'N';
        break;
    }
};

void go_forward(struct ant *ptr)
{
    // Zmiana pozycji (x,y) o jeden piksel w kierunku, w którym aktualnie mrówka zmierza
    switch (ptr->current_direction)
    {
    case 'N':
        ptr->y_pos += 1;
        break;
    case 'W':
        ptr->x_pos -= 1;
        break;
    case 'S':
        ptr->y_pos -= 1;
        break;
    case 'E':
        ptr->x_pos += 1;
        break;
    }
};

void check_ant_stop(struct ant *ptr)
{
    // Sprawdzenie, czy mrówka nie wyszła poza obszar planszy,
    // jeżeli wyszła, jest na stałe zatrzymywana
    if (ptr->x_pos >= WIDTH ||
        ptr->y_pos >= HEIGHT ||
        ptr->x_pos < 0 ||
        ptr->y_pos < 0)

        ptr->moving = 0;
};

int generate_random_int(int min_int, int max_int)
{
    int random_number = rand() % ((max_int + 1) - min_int) + min_int;
    return random_number;
};

void ant_init(struct ant *ptr)
{
    // Ustawienie losowych początkowych parametrów mrówki, czyli
    // pozycja (x, y) oraz kolor (r, g, b)
    // oraz ustawienie kierunku początkowego mrówki
    ptr->x_pos = generate_random_int(0, WIDTH);
    ptr->y_pos = generate_random_int(0, HEIGHT);
    ptr->rgb_r = generate_random_int(0, 255);
    ptr->rgb_g = generate_random_int(0, 255);
    ptr->rgb_b = generate_random_int(0, 255);
    ptr->current_direction = 'E';

    // Początkowo każda mrówka może się poruszać
    ptr->moving = 1;
}

void ant_walk(struct ant *ptr)
{
    // Sprawdzane są warunki, dotyczące możliwości ruchu mrówki oraz
    // kierunku
    if (ptr->moving == 1)
    {
        // W pierwszej kolejności sprawdzane jest czy mrówka może się jeszce poruszać
        if (area[ptr->x_pos][ptr->y_pos] == 0)
        {
            // Jeżeli plansza w miejscu przebywania mrówki jest oznaczona jako niezamalowana, to zostaje oznaczona jako zamalowana
            area[ptr->x_pos][ptr->y_pos] = 1;

            // rysowany jest piksel w kolorze, który dana mrówka ma zdefiniowany
            turtle_set_fill_color(ptr->rgb_r, ptr->rgb_g, ptr->rgb_b);

            turtle_fill_pixel((ptr->x_pos) - WIDTH / 2, (ptr->y_pos) - HEIGHT / 2);

            // mrówka skręca w lewo a następnie porusza się do przodu
            turn_left(ptr);
            go_forward(ptr);
        }
        else if (area[ptr->x_pos][ptr->y_pos] == 1)
        {
            // Jeżeli plansza w miejscu przebywania mrówki jest oznaczona jako zamalowana, to zostaje oznaczona jako niezamalowana
            area[ptr->x_pos][ptr->y_pos] = 0;

            turtle_set_fill_color(255, 255, 255);
            turtle_fill_pixel((ptr->x_pos) - WIDTH / 2, (ptr->y_pos) - HEIGHT / 2);

            // mrówka skręca w prawo a następnie porusza się do przodu
            turn_right(ptr);
            go_forward(ptr);
        }
    }
}