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

    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void on_fruitClick(int x, int y);

private:
    Ui::MainWindow *ui;

    // Scene for the game grid
    QGraphicsScene* scene_;
    QGraphicsPixmapItem* fruitItem_;

    QTimer timer;

    // Margins for the drawing area (the graphicsView object)
    // You can change the values as you wish
    const int TOP_MARGIN = 150;
    const int LEFT_MARGIN = 100;

    // Size of a square containing a fruit
    const int SQUARE_SIDE = 40; // give your own value here
    // Number of vertical cells (places for fruits)
    const int ROWS = 10; // give your own value here
    // Number of horizontal cells (places for fruits)
    const int COLUMNS = 15; // give your own value here

    // Constants describing scene coordinates
    const int BORDER_UP = 0;
    const int BORDER_DOWN = SQUARE_SIDE * ROWS;
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = SQUARE_SIDE * COLUMNS;

    const std::vector<std::string>
            fruits = {"cherries", "strawberry", "orange", "pear",
                      "apple", "bananas", "grapes", "eggplant"};

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
                     BANANAS,
                     GRAPES,
                     EGGPLANT,
                     NUMBER_OF_FRUITS};

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
    };

    void init_grid();
    void scene_add_item(int x, int y, Fruit_data &data, bool errorChk = false);
    void init_3btb();
    void try_change_fruits(int x1, int y1, int x2, int y2);

    void update_fruit(int x, int y);


    std::vector<std::vector<Fruit_data>> grid;
};
#endif // MAINWINDOW_HH
