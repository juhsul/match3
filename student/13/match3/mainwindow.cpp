#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QDebug>
#include <QPixmap>
#include <vector>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // We need a graphics scene in which to draw rectangles
    scene_ = new QGraphicsScene(this);

    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(LEFT_MARGIN, TOP_MARGIN,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene_);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a fruit is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    int seed = time(0); // You can change seed value for testing purposes
    //seed = 9;
    randomEng_.seed(seed);
    distr_ = std::uniform_int_distribution<int>(0, NUMBER_OF_FRUITS - 1);
    distr_(randomEng_); // Wiping out the first random number (which is almost always 0)

    init_titles();

    connect(ui->new_game_btn, &QPushButton::clicked,
            this, &MainWindow::init_grid);
    connect(ui->delay_box, &QCheckBox::stateChanged,
            this, &MainWindow::on_delay_box_click);

    ui->delay_box->setChecked(true);
    // More code perhaps needed
    // draw_fruit();
    //init_grid();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete fruitItem_;
    delete new_game;
}

void MainWindow::init_titles()
{
    // Displaying column titles, starting from A
    for(int i = 0, letter = 'A'; i < COLUMNS; ++i, ++letter)
    {
        int shift = 5;
        QString letter_string = "";
        letter_string.append(letter);
        QLabel* label = new QLabel(letter_string, this);
        label->setGeometry(LEFT_MARGIN + shift + i * SQUARE_SIDE,
                           TOP_MARGIN - SQUARE_SIDE,
                           SQUARE_SIDE, SQUARE_SIDE);
    }

    // Displaying row titles, starting from A
    for(int i = 0, letter = 'A'; i < ROWS; ++i, ++letter)
    {
        QString letter_string = "";
        letter_string.append(letter);
        QLabel* label = new QLabel(letter_string, this);
        label->setGeometry(LEFT_MARGIN - SQUARE_SIDE,
                           TOP_MARGIN + i * SQUARE_SIDE,
                           SQUARE_SIDE, SQUARE_SIDE);
    }
}

void MainWindow::draw_fruit()
{
    // Vector of fruits
    const std::vector<std::string>
            fruits = {"cherries", "strawberry", "orange", "pear", "apple",
                      "bananas", "tomato", "grapes", "eggplant"};

    // Defining where the images can be found and what kind of images they are
    const std::string PREFIX(":/");
    const std::string SUFFIX(".png");

    // Converting image (png) to a pixmap
    int i = 0; // try different values in 0 <= i < fruits.size()
    std::string filename = PREFIX + fruits.at(i) + SUFFIX;
    QPixmap image(QString::fromStdString(filename));

    // Scaling the pixmap
    image = image.scaled(SQUARE_SIDE, SQUARE_SIDE);

    // Setting the pixmap for a new label
    QLabel* label = new QLabel("test", this);
    label->setGeometry(LEFT_MARGIN + 2 * SQUARE_SIDE,
                       TOP_MARGIN + 2 * SQUARE_SIDE,
                       SQUARE_SIDE, SQUARE_SIDE);
    label->setPixmap(image);
}

void MainWindow::init_grid()
{
    if (not(grid.empty()))
        for (int i = 0; i < COLUMNS; i++)
            for (int j = 0; j < ROWS; j++)
            {
                scene_->removeItem(grid[i][j].image);
                delete grid[i][j].button;
            }
    grid.clear();

    for (int i = 0; i < COLUMNS; i++)
    {
        std::vector<Fruit_data> columVector;

        for (int j = 0; j < ROWS; j++)
        {
            Fruit_data newFruitData;
            newFruitData.kind = Fruit_kind(distr_(randomEng_));

            newFruitData.button = new QPushButton;
            newFruitData.button->setGeometry(i * SQUARE_SIDE,
                                             j * SQUARE_SIDE,
                                             SQUARE_SIDE, SQUARE_SIDE);
            scene_->addWidget(newFruitData.button);
            connect(newFruitData.button, &QPushButton::clicked,
                    this, [this, i, j]{MainWindow::on_fruitClick(i, j);});

            newFruitData.image = new QGraphicsPixmapItem;
            newFruitData.image->setZValue(10000);

            scene_add_item(i, j, newFruitData);

            columVector.push_back(newFruitData);
        }
        grid.push_back(columVector);
    }
    init_3btb();
}

void MainWindow::scene_add_item(int x, int y, MainWindow::Fruit_data &data,
                                bool errorChk)
{
    if (errorChk){
        qDebug() << "errorChk";
        scene_->removeItem(data.image);}

    // Converting image (png) to a pixmap
    // int i = 0; // try different values in 0 <= i < fruits.size()
    std::string filename = PREFIX + fruits.at(data.kind) + SUFFIX;
    QPixmap image(QString::fromStdString(filename));

    // Scaling the pixmap
    image = image.scaled(SQUARE_SIDE, SQUARE_SIDE);

    // newFruitData.image = image;


    qDebug() << x << " " << y;
    data.image->setPixmap(image);
    data.image->setPos(SQUARE_SIDE * x, SQUARE_SIDE * y);
    scene_->addItem(data.image);
}

void MainWindow::init_3btb()
{
    bool correction = false;

    for (int i = 0; i < COLUMNS; i++)
    {
        for (int j = 2; j < ROWS; j++)
        {
            if (grid[i][j - 1].kind == grid[i][j - 2].kind
                    and grid[i][j].kind == grid[i][j - 1].kind)
            {
                while (grid[i][j].kind == grid[i][j - 1].kind)
                {
                    correction = true;
                    grid[i][j].kind = Fruit_kind(distr_(randomEng_));
                }

                scene_add_item(i, j, grid[i][j], true);
            }
        }
    }

    for (int i = 2; i < COLUMNS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            if (grid[i - 1][j].kind == grid[i - 2][j].kind
                    and grid[i][j].kind == grid[i - 1][j].kind)
            {
                while (grid[i][j].kind == grid[i - 1][j].kind)
                {
                    correction = true;
                    grid[i][j].kind = Fruit_kind(distr_(randomEng_));
                }

                scene_add_item(i, j, grid[i][j], true);
            }
        }
    }

    if (correction)
        init_3btb();
}

void MainWindow::on_fruitClick(int x, int y)
{
    qDebug() << x << y;

    for (int i = 0; i < COLUMNS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            if (grid[i][j].clicked)
            {
                try_change_fruits(i, j, x, y);
                grid[i][j].clicked = false;
                grid[i][j].button->setStyleSheet("background-color: ");
                return;
            }
        }
    }

    grid[x][y].clicked = true;
    grid[x][y].button->setStyleSheet("background-color: #7899ff");
}

void MainWindow::on_delay_box_click()
{
    if (ui->delay_box->isChecked())
        DELAY = 210;
    else
        DELAY = 0;
}


void MainWindow::try_change_fruits(int x1, int y1, int x2, int y2)
{
    if (x1 == x2 and y1 == y2)
    {
        qDebug() << "sama paino";
        return;
    }

    if (abs(x1 - x2) > 1 or abs(y1 - y2) > 1)
    {

        qDebug() << "väärä etäisyys";
        return;

    }

    if (abs(x1 - x2) == 1 and abs(y1 - y2) == 1)
    {
        qDebug() << "vino";
        return;
    }

    if (grid[x1][y1].isEmpty == true or grid[x2][y2].isEmpty == true)
    {
        qDebug() << "tyhjä ruutu";
        return;
    }

    grid[x1][y1].image->moveBy(SQUARE_SIDE * (x2 - x1),
                               SQUARE_SIDE * (y2 - y1));
    grid[x2][y2].image->moveBy(SQUARE_SIDE * (x1 - x2),
                               SQUARE_SIDE * (y1 - y2));


    QGraphicsPixmapItem* tempImage = grid[x1][y1].image;
    grid[x1][y1].image = grid[x2][y2].image;
    grid[x2][y2].image = tempImage;

    Fruit_kind tempKind = grid[x1][y1].kind;
    grid[x1][y1].kind = grid[x2][y2].kind;
    grid[x2][y2].kind = tempKind;

    bool match = false;

    if (try_remove_3btb(x1, y1))
        match = true;

    if (try_remove_3btb(x2, y2))
        match = true;

    if (not(match))
        QTimer::singleShot(DELAY, this, [this, x1, y1, x2, y2]
        {
            grid[x1][y1].image->moveBy(SQUARE_SIDE * (x2 - x1),
                                       SQUARE_SIDE * (y2 - y1));
            grid[x2][y2].image->moveBy(SQUARE_SIDE * (x1 - x2),
                                       SQUARE_SIDE * (y1 - y2));


            QGraphicsPixmapItem* tempImage = grid[x1][y1].image;
            grid[x1][y1].image = grid[x2][y2].image;
            grid[x2][y2].image = tempImage;

            Fruit_kind tempKind = grid[x1][y1].kind;
            grid[x1][y1].kind = grid[x2][y2].kind;
            grid[x2][y2].kind = tempKind;
        });

    QTimer::singleShot(DELAY * 2, this, SLOT(drop_fruits()));
}
// TODO return bool jos ei onnistu niin saa animaation vaihdosta jos onnistuu
// siirtyy alas putoamis hommaan
bool MainWindow::try_remove_3btb(int x, int y)
{
    int xFunc1 = x;
    int xFunc2 = x;
    int yFunc1 = y;
    int yFunc2 = y;
    bool returnValue = false;

    for (int i = x; grid[x][y].kind == grid[i][y].kind; i--)
    {
        if (i == 0){
            xFunc1 = i;
            break;}
        xFunc1 = i;
    }
    for (int i = x; grid[x][y].kind == grid[i][y].kind; i++)
    {
        if (i == COLUMNS - 1){
            xFunc2 = i;
            break;}
        xFunc2 = i;
    }

    // Vaakarivin poisto
    if (xFunc2 - xFunc1 >= 2)
    {
        QTimer::singleShot(DELAY, this, [this, xFunc1, xFunc2, y]
        {delete_3btb(xFunc1, xFunc2, y, true);});
        //delete_3btb(xFunc1, xFunc2, y, true);
        returnValue = true;

    }

    for (int i = y; grid[x][y].kind == grid[x][i].kind; i--)
    {
        if (i == 0){
            yFunc1 = i;
            break;}
        yFunc1 = i;
    }
    for (int i = y; grid[x][y].kind == grid[x][i].kind; i++)
    {
        if (i == ROWS - 1){
            yFunc2 = i;
            break;}
        yFunc2 = i;
    }

    // Pystyrivin poisto
    if (yFunc2 - yFunc1 >= 2)
    {
        QTimer::singleShot(DELAY, this, [this, yFunc1, yFunc2, x]
        {delete_3btb(yFunc1, yFunc2, x, false);});
        //delete_3btb(yFunc1, yFunc2, x, false);
        returnValue = true;
    }

    return returnValue;
}

void MainWindow::drop_fruits()
{
    for (int i = COLUMNS - 1; i > -1; i--)
    {
        for (int j = ROWS - 1; j > -1; j--)
        {
            int l = j;
            if (grid[i][j].isEmpty == true)
            {
                for (int k = j; k > -1; k--)
                {
                    if (grid[i][k].isEmpty == false)
                    {

                        grid[i][k].image->moveBy(0, SQUARE_SIDE * (l - k));
                        grid[i][l].isEmpty = false;
                        grid[i][k].isEmpty = true;
                        grid[i][l].image = grid[i][k].image;
                        grid[i][l].kind = grid[i][k].kind;
                        l--;
                    }
                }
                break;
            }
        }
    }
}

/* TODO: virhetarkastelu, sama paino
 * tai ei vierekkäin paino, jos vaihto niin poisto jos yli 3 ja tiputus alas
 * timerin kanssa -> ei saa olla välitön.
*/

void MainWindow::delete_3btb(int a, int b, int static_axis, bool x_axis)
{

    for (int i = b; i >= a; i--)
    {
        if (x_axis)
        {
            scene_->removeItem(grid[i][static_axis].image);
            grid[i][static_axis].isEmpty = true;
        }
        else
        {
            scene_->removeItem(grid[static_axis][i].image);
            grid[static_axis][i].isEmpty = true;
        }
    }

}
