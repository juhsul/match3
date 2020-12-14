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
    void init_grid();
    void on_fruit_click(int x, int y);
    void on_delay_box_click();
    void delete_3btb(int a, int b, int static_axis, bool x_axis);
    void drop_fruits();
    void on_slider_change();
    void on_slider_change_delay();
    void on_timeout_game_time();
    void on_slider_change_fruits();

private:
    Ui::MainWindow *ui;

    // Scene for the game grid
    QGraphicsScene* scene_;
    QGraphicsPixmapItem* fruitItem_;

    QPushButton* new_game;

    QTimer* timer;

    // Margins for the drawing area (the graphicsView object)
    // You can change the values as you wish
    const int TOP_MARGIN = 150;
    const int LEFT_MARGIN = 50;

    // Size of a square containing a fruit
    const int SQUARE_SIDE = 40; // give your own value here
    // Number of vertical cells (places for fruits)
    int ROWS = 10; // give your own value here
    // Number of horizontal cells (places for fruits)
    int COLUMNS = 15; // give your own value here
    const int MAX_ROWS = 20;
    const int MAX_COLUMNS = 45;
    const int MIN_ROWS = 5;
    const int MIN_COLUMNS = 5;

    // Constants describing scene coordinates
    const int BORDER_UP = 0;
    const int BORDER_LEFT = 0;

    int delay;
    int DELAY_CONST = 250;
    const int MAX_DELAY = 1000;
    const int MIN_DELAY = 50;

    int number_of_fruits = 7;

    bool additional_fruits = false;

    const std::vector<std::string>
            fruits = {"cherries", "strawberry", "orange", "pear",
                      "apple", "grapes", "eggplant", "bananas"};

    // Defining where the images can be found and what kind of images they are
    const std::string PREFIX = ":/";
    const std::string SUFFIX = ".png";

    // Constants for different fruits and the number of them
    // Add/remove fruits as you wish, or you can remove the whole enum type
    // Actually only the value NUMBER_OF_FRUITS is needed in the
    // template code
    enum Fruit_kind {CHERRIES,
                     STRAWBERRY,
                     ORANGE,
                     PEAR,
                     APPLE,
                     GRAPES,
                     EGGPLANT,
                     BANANAS,
                     FRUITS_LAST};

    // For randomly selecting fruits for the grid
    std::default_random_engine randomEng_;
    std::uniform_int_distribution<int> distr_;

    // Writes the titles for the grid rows and columns
    void init_titles();

    // Draws a single fruit near the right bottom corner of the grid
    // At the moment, this function is not called in the template,
    // but try to do so, if you want to use real fruits instead of rectangles.
    void draw_fruit();

    struct Fruit_data
    {
        Fruit_kind kind;
        QGraphicsPixmapItem* image;
        QPushButton* button;
        bool clicked = false;
        bool isEmpty = false;
    };

    void scene_add_item(int x, int y, Fruit_data &data, bool errorChk = false);
    void init_3btb();
    void try_change_fruits(int x1, int y1, int x2, int y2);
    bool try_remove_3btb(int x, int y);

    void update_fruit(int x, int y);

    void enable_buttons(bool enable);

    void drop_additional_fruits();

    int number_of_games = 0;
    int seconds;
    int minutes;

    std::vector<std::vector<Fruit_data>> grid;
};
#endif // MAINWINDOW_HH
