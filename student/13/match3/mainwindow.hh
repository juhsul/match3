/* Match3
 *
 * Kuvaus ohjelman toiminnasta löytyy instructions.txt tiedostosta
 *
 *
 * Ohjelman kirjoittaja
 * Nimi: Juho Sulkonen
 * Opiskelijanumero: 283145
 * Käyttäjätunnus: sulkonen
 * E-Mail: juho.sulkonen@tuni.fi
 * Palautteen kieli: fi
 */

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPushButton>
#include <vector>
#include <deque>
#include <random>
#include <QTimer>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Alustaa uuden pelialueen
    void init_grid();

    // Tarkastaa onko aikaisempia nappuloita painettu, jos on niin kutsuu
    // try_change_fruits -funktiota. Jos aikaisempia nappeja ei ole painettu,
    // merkitsee kyseisen napin painetuksi.
    // Parametrit: Napin koordinaatit
    void on_fruit_click(int x, int y);

    // Poistaa perättäiset hedelmät
    // Param1: pienempi ääriarvo samojen hedelmien jonosta
    // Param2: suurempi ääriarvo samojen hedelmien jonosta
    // Param3: staattinen akseli
    // Param4: bool onko staattiinen akseli X-akseli
    void delete_3btb(int a, int b, int static_axis, bool x_axis);

    // Hedelmien pudotus, kun pelialueella on tyhjiä alueita. Tarkastaa myös
    // onko pudotuksen seurauksena tullut uusia matcheja
    void drop_fruits();

    // Muuttaa muuttujaa delay sitä mukaa kun delay_box vaihtaa arvoa
    void on_delay_box_click();

    // Muuttaa muuttujia COLUMNS ja ROWS sitä mukaan kun horizontal_slider_x
    // ja horizontal_slider_y vaihtavat arvoa. Muuttaa myös label_x ja
    // label_y arvon slidereiden arvoon
    void on_slider_change();

    // Muuttaa muuttujaa DELAY_CONST sitä mukaa kun horizontal_slider_delay
    // vaihtaa arvoa. Vaihtaa myös label_delay arvon samaan arvoon.
    void on_slider_change_delay();

    // Laskee pelissä kulunutta aikaa
    void on_timeout_game_time();

    // Muuttaa erilaisten hedelmien määrää
    void on_slider_change_fruits();

private:
    Ui::MainWindow *ui;

    QGraphicsScene* scene_;

    QTimer* timer;

    // Reunat graphicsView objektille
    const int TOP_MARGIN = 150;
    const int LEFT_MARGIN = 50;

    // Hedelmän reunojen koko
    const int SQUARE_SIDE = 40;
    // Pelialueen koko Y-akselilla
    int ROWS = 10;
    // Pelialueen koko X-akselilla
    int COLUMNS = 15;
    // Maksimi ja minimi arvot pelialueelle
    const int MAX_ROWS = 20;
    const int MAX_COLUMNS = 45;
    const int MIN_ROWS = 5;
    const int MIN_COLUMNS = 5;

    int delay;
    int DELAY_CONST = 250;
    // Maksimi ja minimi arvot viiveelle
    const int MAX_DELAY = 1000;
    const int MIN_DELAY = 50;

    int number_of_fruits = 7;

    // Vektori jossa pidetään hedelmien nimitiedostojen nimiä
    const std::vector<std::string>
            fruits = {"cherries", "strawberry", "orange", "pear",
                      "apple", "grapes", "eggplant", "bananas"};

    // Määritetään mistä hedelmätiedostot löytää ja mitä tyyppiä ne on
    const std::string PREFIX = ":/";
    const std::string SUFFIX = ".png";

    // Arvot eri hedelmille
    enum Fruit_kind {CHERRIES,
                     STRAWBERRY,
                     ORANGE,
                     PEAR,
                     APPLE,
                     GRAPES,
                     EGGPLANT,
                     BANANAS,
                     FRUITS_LAST};

    // Satunnaislukugeneraattori hedelmien valintaan
    std::default_random_engine randomEng_;
    std::uniform_int_distribution<int> distr_;

    // Hedelmien tallennustiedot
    struct Fruit_data
    {
        // Hedelmän tyyppi
        Fruit_kind kind;
        // Kuva hedelmälle
        QGraphicsPixmapItem* image;
        // Nappi hedelmän taakse
        QPushButton* button;
        // Booleani siitä onko kyseistä hedelmää klikattu
        bool clicked = false;
        // Booleani siitä onko ruutu tyhjä
        bool isEmpty = false;
    };

    // Lisää sceneen pixmapin
    // Param1: X-koordinaatti johon lisätään hedelmä
    // Param2: Y-koordinaatti johon lisätään hedelmä
    // Param3: data struct lisättävälle kohdalle
    // Param4: onko lisäys virhetarkastuksen jälkeen
    void scene_add_item(int x, int y, Fruit_data &data, bool errorChk = false);

    // Tarkastaa onko pelialueella kolmea samanlaista hedelmää peräkkäin.
    // Jos on korjaa ne uusiksi jotka eroavat vanhasta
    void init_3btb();

    // Tarkastaa syntyykö vaihdetuista hedelmistä kolmen suoria. Kutsuu
    // funktiota try_remove_3btb, joka poistaa syntyneet kolmen suorat. Jos on
    // syntynyt kolmen suoria tiputetaan uudet hedelmät alas.
    // Param1: Ensimmäisen hedelmän X-koordinaatti
    // Param2: Ensimmäisen hedelmän Y-koordinaatti
    // Param3: Toisen hedelmän X-koordinaatti
    // Param4: Toisen hedelmän Y-koordinaatti
    void try_change_fruits(int x1, int y1, int x2, int y2);

    // Selvittää syntyykö annetuissa koordinaateissa kolmen suoria. Jos syntyy
    // kutsuu funktiota delete_3btb.
    // Param1: X-koordinaatti hedelmälle
    // Param2: Y-koordinaatti hedelmälle
    // Palauttaa arvon siitä toteutuiko poisto
    bool try_remove_3btb(int x, int y);

    // Ottaa käyttöön tai poistaa käytöstä pelialueen nappulat
    // Param: totuusarvo otetaanko käyttöön
    void enable_buttons(bool enable);

    int number_of_games = 0;
    int seconds;
    int minutes;

    // Vektori mihin ruudukko tallennetaan
    std::vector<std::vector<Fruit_data>> grid;
};
#endif // MAINWINDOW_HH
