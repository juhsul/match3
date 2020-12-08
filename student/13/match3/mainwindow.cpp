#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QDebug>
#include <QPixmap>
#include <vector>

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
    seed = 9;
    randomEng_.seed(seed);
    distr_ = std::uniform_int_distribution<int>(0, NUMBER_OF_FRUITS - 1);
    distr_(randomEng_); // Wiping out the first random number (which is almost always 0)

    init_titles();

    // More code perhaps needed
    // draw_fruit();
    init_grid();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fruitItem_;
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
//    const std::vector<std::string>
//            fruits = {"cherries", "strawberry", "orange", "pear",
//                      "apple", "bananas", "grapes", "eggplant"};

//    // Defining where the images can be found and what kind of images they are
//    const std::string PREFIX(":/");
//    const std::string SUFFIX(".png");

    for (int i = 0; i < COLUMNS; i++)
    {
        std::vector<Fruit_data> columVector;

        for (int j = 0; j < ROWS; j++)
        {
            Fruit_data newFruitData;
            //newFruitData.kind = Fruit_kind(distr_(randomEng_));
            newFruitData.kind = Fruit_kind(distr_(randomEng_));

//            if (j > 1)
//                if (columVector[j - 1].kind == columVector[j - 2].kind)
//                    while (newFruitData.kind == columVector[j - 1].kind)
//                        newFruitData.kind = Fruit_kind(distr_(randomEng_));


            newFruitData.image = new QGraphicsPixmapItem;


            scene_add_item(i, j, newFruitData);

//            // Converting image (png) to a pixmap
//            // int i = 0; // try different values in 0 <= i < fruits.size()
//            std::string filename = PREFIX + fruits.at(newFruitData.kind) + SUFFIX;
//            QPixmap image(QString::fromStdString(filename));

//            // Scaling the pixmap
//            image = image.scaled(SQUARE_SIDE, SQUARE_SIDE);

//            // newFruitData.image = image;


//            newFruitData.image->setPixmap(image);
//            newFruitData.image->setPos(SQUARE_SIDE * i, SQUARE_SIDE * j);
//            scene_->addItem(newFruitData.image);



            // newFruitData.image = fruitItem_;
            columVector.push_back(newFruitData);

            //Sleep(5);
        }

        grid.push_back(columVector);
    }
    init_3btb();

//    for (int i = 2; i < COLUMNS; i++)
//    {
//        for (int j = 2; j < COLUMNS; j++)
//        {
//            if (grid[i][j - 1].kind == grid[i][j - 2].kind
//                    and grid[i][j].kind == grid[i][j - 1].kind)

//        }
    //    }
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

//void MainWindow::update_fruit(int x, int y)
//{
//    fruitItem_->setPixmap(grid[x][y].image);
//    fruitItem_->setPos(SQUARE_SIDE * x, SQUARE_SIDE * y);
//    scene_->addItem(fruitItem_);
//}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    // changing the color of the circle between red and blue
//    if(event->key() == Qt::Key_S)
//    {
//        grid[0][0].kind = Fruit_kind(0);
//        QPixmap image(QString::fromStdString(":/apple.png"));
//        image = image.scaled(SQUARE_SIDE, SQUARE_SIDE);
//        grid[0][0].image = image;
//        fruitItem_ = new QGraphicsPixmapItem;
//        fruitItem_->setPixmap(grid[0][0].image);
//        fruitItem_->setPos(SQUARE_SIDE * 0, SQUARE_SIDE * 0);
//        scene_->addItem(fruitItem_);
//    }

    if (event->key() == Qt::Key_D)
    {
        grid[0][0].image->moveBy(SQUARE_SIDE * 1, SQUARE_SIDE * 1);
        grid[1][1].image->moveBy(SQUARE_SIDE * -1, SQUARE_SIDE * -1);

        QGraphicsPixmapItem* tempImage = grid[0][0].image;
        grid[0][0].image = grid[1][1].image;
        grid[1][1].image = tempImage;

        Fruit_kind tempKind = grid[0][0].kind;
        grid[0][0].kind = grid[1][1].kind;
        grid[1][1].kind = tempKind;

//        if (grid[0][0].image != grid[1][1].image)
//        {
//            grid[0][0].image->moveBy(5, 5);
//            qDebug() << "eri kuva";
//        }

//        if (grid[0][0].kind != grid[1][1].kind)
//            qDebug() << "eri kind";

//        if (grid[0][0].image == grid[1][1].image)
//        {
//            grid[0][0].image->moveBy(5, 5);
//            qDebug() << "sama kuva";
//        }

//        if (grid[0][0].kind == grid[1][1].kind)
//            qDebug() << "sama kind";
        //scene_->update();
        //scene_->addItem(grid[1][1].image);
    }

//    if (event->key() == Qt::Key_S)
//        init_grid();
}
