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

    int seed = time(0);

    // Satunnaislukugeneraattori hedelmien luomiselle
    randomEng_.seed(seed);
    distr_ = std::uniform_int_distribution<int>(0, number_of_fruits);
    distr_(randomEng_);

    connect(ui->new_game_btn, &QPushButton::clicked,
            this, &MainWindow::init_grid);
    connect(ui->delay_box, &QCheckBox::stateChanged,
            this, &MainWindow::on_delay_box_click);

    connect(ui->horizontal_slider_x, &QSlider::valueChanged,
            this, &MainWindow::on_slider_change);
    connect(ui->horizontal_slider_y, &QSlider::valueChanged,
            this, &MainWindow::on_slider_change);
    connect(ui->horizontal_slider_delay, &QSlider::valueChanged,
            this, &MainWindow::on_slider_change_delay);
    connect(ui->horizontal_slider_fruits, &QSlider::valueChanged,
            this, &MainWindow::on_slider_change_fruits);

    ui->horizontal_slider_x->setMaximum(MAX_COLUMNS);
    ui->horizontal_slider_y->setMaximum(MAX_ROWS);
    ui->horizontal_slider_x->setMinimum(MIN_COLUMNS);
    ui->horizontal_slider_y->setMinimum(MIN_ROWS);
    ui->horizontal_slider_delay->setMaximum(MAX_DELAY);
    ui->horizontal_slider_delay->setMinimum(MIN_DELAY);

    ui->horizontal_slider_fruits->setMaximum(FRUITS_LAST);
    ui->horizontal_slider_fruits->setMinimum(4);

    ui->horizontal_slider_x->setValue(15);
    ui->horizontal_slider_y->setValue(10);
    ui->horizontal_slider_delay->setValue(250);
    ui->horizontal_slider_fruits->setValue(7);

    ui->delay_box->setChecked(true);

    // Timerin luominen pelin ajanlaskuun ja fontin säätäminen sille
    timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_timeout_game_time);
    QFont font_label_time = ui->label_time->font();
    font_label_time.setPointSize(15);
    ui->label_time->setFont(font_label_time);
    ui->label_time_title->setFont(font_label_time);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
    for (int i = 0; i < COLUMNS; i++)
        for (int j = 0; j < ROWS; j++)
            delete grid[i][j].image;
}

void MainWindow::init_grid()
{
    // Timerin nollaaminen
    seconds = 0;
    minutes = 0;
    timer->stop();
    ui->label_time->setText(QString::fromStdString("0:0"));

    // Tästä puhuinkin jo instruction.txt tiedostossa, kaiken järjen mukaan
    // pitäisi toimia, mutta ei toimi
    ui->new_game_btn->setDisabled(true);

    // scenen ja pixmapin deletoiminen
    if (number_of_games > 0)
        for (int i = 0; i < COLUMNS; i++)
            for (int j = 0; j < ROWS; j++)
                delete grid[i][j].image;
        delete scene_;
    number_of_games++;

    scene_ = new QGraphicsScene(this);
    scene_->update();

    ui->graphicsView->setGeometry(LEFT_MARGIN, TOP_MARGIN,
                                  SQUARE_SIDE * COLUMNS + 2,
                                  SQUARE_SIDE * ROWS + 2);

    ui->graphicsView->setScene(scene_);

    scene_->setSceneRect(0, 0, SQUARE_SIDE * COLUMNS - 1,
                         SQUARE_SIDE * ROWS - 1);

    grid.clear();

    // For loopeissa muodostetaan pelialueen hedelmät. column_vectoriin
    // tallennetaan yksi sarake jonka jälkeen se tallennetaan grid vectoriin.
    std::vector<Fruit_data> colum_vector;
    for (int i = 0; i < COLUMNS; i++)
    {
        colum_vector.clear();
        for (int j = 0; j < ROWS; j++)
        {
            // Hedelmän tyypin päärittäminen
            Fruit_data newFruitData;
            newFruitData.kind = Fruit_kind(distr_(randomEng_));

            // Napin luominen hedelmälle
            newFruitData.button = new QPushButton;
            newFruitData.button->setGeometry(i * SQUARE_SIDE,
                                             j * SQUARE_SIDE,
                                             SQUARE_SIDE, SQUARE_SIDE);
            scene_->addWidget(newFruitData.button);
            connect(newFruitData.button, &QPushButton::clicked,
                    this, [this, i, j]{MainWindow::on_fruit_click(i, j);});

            newFruitData.image = new QGraphicsPixmapItem;
            // Määritetään hedelmän Z-akselin arvo, että ei ole napin takana
            newFruitData.image->setZValue(10000);

            // Hedelmän lisääminen sceneen erillisellä funktiolla
            scene_add_item(i, j, newFruitData);

            colum_vector.push_back(newFruitData);
        }
        grid.push_back(colum_vector);
    }

    // Kolmen peräkkäisen tarkastus
    init_3btb();

    ui->new_game_btn->setEnabled(true);

    timer->start();
}

void MainWindow::scene_add_item(int x, int y, MainWindow::Fruit_data &data,
                                bool errorChk)
{
    if (errorChk)
        scene_->removeItem(data.image);

    // Kuvan muuttaminen pixmapiksi
    std::string filename = PREFIX + fruits.at(data.kind) + SUFFIX;
    QPixmap image(QString::fromStdString(filename));

    // Skaalaus
    image = image.scaled(SQUARE_SIDE, SQUARE_SIDE);

    // Hedelmän asettaminen scenelle
    data.image->setPixmap(image);
    data.image->setPos(SQUARE_SIDE * x, SQUARE_SIDE * y);
    scene_->addItem(data.image);
}

void MainWindow::init_3btb()
{
    bool correction = false;
    int number_of_corrections = 0;

    // Vaakatasossa peräkkäisten kolmen tarkastus
    for (int i = 0; i < COLUMNS; i++)
    {
        for (int j = 2; j < ROWS; j++)
        {
            // Kolmen peräkkäisen tarkastelu
            if (grid[i][j - 1].kind == grid[i][j - 2].kind
                    and grid[i][j].kind == grid[i][j - 1].kind)
            {
                while (grid[i][j].kind == grid[i][j - 1].kind)
                {
                    // Jos jää looppaamaan liikaa korjauksia tehdään kokonaan
                    // uusi pelialue
                    if (number_of_corrections > 100)
                    {
                        init_grid();
                        return;
                    }
                    correction = true;
                    grid[i][j].kind = Fruit_kind(distr_(randomEng_));
                    number_of_corrections++;
                }

                scene_add_item(i, j, grid[i][j], true);
            }
        }
    }

    // Pystysuunnassa peräkkäisten kolmen tarkastus
    for (int i = 2; i < COLUMNS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            if (grid[i - 1][j].kind == grid[i - 2][j].kind
                    and grid[i][j].kind == grid[i - 1][j].kind)
            {
                while (grid[i][j].kind == grid[i - 1][j].kind)
                {
                    if (number_of_corrections > 100)
                    {
                        init_grid();
                        return;
                    }
                    correction = true;
                    grid[i][j].kind = Fruit_kind(distr_(randomEng_));
                    number_of_corrections++;
                }

                scene_add_item(i, j, grid[i][j], true);
            }
        }
    }

    // Jos tapahtui korjaus, tarkastetaan vielä kerran
    if (correction)
        init_3btb();
}

void MainWindow::on_fruit_click(int x, int y)
{
    enable_buttons(false);

    // Nappien läpikäyminen ja tarkastus löytyykö jo valmiiksi painettua
    // nappia
    for (int i = 0; i < COLUMNS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            if (grid[i][j].clicked)
            {
                // Hedelmien vaihdon kokeileminen funktiolla
                try_change_fruits(i, j, x, y);
                grid[i][j].clicked = false;
                grid[i][j].button->setStyleSheet("background-color: ");
                return;
            }
        }
    }

    // Jos ei löydy valmiiksi painettua neppia niin tehdään sellainen
    grid[x][y].clicked = true;
    grid[x][y].button->setStyleSheet("background-color: #7899ff");
    enable_buttons(true);
}

void MainWindow::on_delay_box_click()
{
    if (ui->delay_box->isChecked())
        delay = DELAY_CONST;
    else
        delay = 0;
}


void MainWindow::try_change_fruits(int x1, int y1, int x2, int y2)
{
    // Jos ensiksi painettu nappi on sama nappi jota painettii jälkeen päin
    if (x1 == x2 and y1 == y2)
    {
        enable_buttons(true);
        return;
    }

    // Jos painetut hedelmät ei ole vierekkäin
    if (abs(x1 - x2) > 1 or abs(y1 - y2) > 1)
    {
        enable_buttons(true);
        return;
    }

    // Painetut hedelmät ovat vinottain toisiinsa nähden
    if (abs(x1 - x2) == 1 and abs(y1 - y2) == 1)
    {
        enable_buttons(true);
        return;
    }

    // Jos on painettu tyhjää ruutua
    if (grid[x1][y1].isEmpty == true or grid[x2][y2].isEmpty == true)
    {
        enable_buttons(true);
        return;
    }

    // Hedelmien paikkojen vaihtaminen
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

    // Selvitetään onko tullut matchia
    if (try_remove_3btb(x1, y1))
        match = true;

    if (try_remove_3btb(x2, y2))
        match = true;

    // Jos ei tullut matchia liikutetaan hedelmät takaisin paikoilleen
    // viiveen jälkeen
    if (not(match))
        QTimer::singleShot(delay, this, [this, x1, y1, x2, y2]
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

            enable_buttons(true);
            return;
        });

    // Jos tuli match niin pudotetaan yllä olevat hedelmät viiveen jälkeen
    else
        QTimer::singleShot(delay * 2, this, [this]
        {
            drop_fruits();
        });

}

bool MainWindow::try_remove_3btb(int x, int y)
{
    if (grid[x][y].isEmpty)
        return false;
    int xFunc1 = x;
    int xFunc2 = x;
    int yFunc1 = y;
    int yFunc2 = y;
    bool returnValue = false;

    // Seuraavat kaksi for looppia selvittävät X-akselilla hedelmäjonon
    // ääriarvot
    for (int i = x; grid[x][y].kind == grid[i][y].kind; i--)
    {
        if (grid[i][y].isEmpty)
            break;
        if (i == 0){
            xFunc1 = i;
            break;}
        xFunc1 = i;
    }
    for (int i = x; grid[x][y].kind == grid[i][y].kind; i++)
    {
        if (grid[i][y].isEmpty)
            break;
        if (i == COLUMNS - 1){
            xFunc2 = i;
            break;}
        xFunc2 = i;
    }

    // Vaakarivin poisto
    if (xFunc2 - xFunc1 >= 2)
    {
        QTimer::singleShot(delay, this, [this, xFunc1, xFunc2, y]
        {delete_3btb(xFunc1, xFunc2, y, true);});
        returnValue = true;
    }

    // Seuraavat kaksi for looppia selvittävät Y-akselilla hedelmäjonon
    // ääriarvot
    for (int i = y; grid[x][y].kind == grid[x][i].kind; i--)
    {
        if (grid[x][i].isEmpty)
            break;
        if (i == 0){
            yFunc1 = i;
            break;}
        yFunc1 = i;
    }
    for (int i = y; grid[x][y].kind == grid[x][i].kind; i++)
    {
        if (grid[x][i].isEmpty)
            break;
        if (i == ROWS - 1){
            yFunc2 = i;
            break;}
        yFunc2 = i;
    }

    // Pystyrivin poisto
    if (yFunc2 - yFunc1 >= 2)
    {
        QTimer::singleShot(delay, this, [this, yFunc1, yFunc2, x]
        {delete_3btb(yFunc1, yFunc2, x, false);});
        returnValue = true;
    }

    return returnValue;
}

void MainWindow::delete_3btb(int a, int b, int static_axis, bool x_axis)
{

    for (int i = b; i >= a; i--)
    {
        if (x_axis)
        {
            if (not(grid[i][static_axis].isEmpty))
            {
                scene_->removeItem(grid[i][static_axis].image);
                grid[i][static_axis].isEmpty = true;
            }
        }
        else
        {
            if (not(grid[static_axis][i].isEmpty))
            {
                scene_->removeItem(grid[static_axis][i].image);
                grid[static_axis][i].isEmpty = true;
            }
        }
    }

}

void MainWindow::enable_buttons(bool enable)
{
    for (int i = 0; i < COLUMNS; i++)
        for (int j =0; j < ROWS; j++)
            grid[i][j].button->setEnabled(enable);
}

void MainWindow::drop_fruits()
{
    // Ruudukon läpikäyminen ja hedelmien pudottaminen
    for (int i = COLUMNS - 1; i > -1; i--)
    {
        for (int j = ROWS - 1; j > -1; j--)
        {
            int l = j;
            // Jos löytyy tyhjä ruutu siirretään seuraava käytössä oleva
            // hedelmä sen yläpuolelta siihen
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

    bool dropped_match = false;

    // Tarkastetaan syntyiko pudotuksen seurauksena matcheja
    for (int i = 0; i < COLUMNS; i++) {
        for (int j = 0; j < ROWS; j++) {
            if (dropped_match)
                try_remove_3btb(i, j);
            else
                dropped_match = try_remove_3btb(i, j);
        }
    }

    // Jos pudotukksen seurauksen syntyi matcheja, niin viiveen jälkeen
    // suoritetaan uusi pudotus
    QTimer::singleShot(delay * 2, this, [this, dropped_match]{
        if (dropped_match)
            drop_fruits();});

    if (not(dropped_match))
        enable_buttons(true);
}

void MainWindow::on_slider_change()
{
    COLUMNS = ui->horizontal_slider_x->value();
    ROWS = ui->horizontal_slider_y->value();

    ui->label_x->setText(QString::number(ui->horizontal_slider_x->value()));
    ui->label_y->setText(QString::number(ui->horizontal_slider_y->value()));
}

void MainWindow::on_slider_change_delay()
{
    DELAY_CONST = ui->horizontal_slider_delay->value();
    delay = DELAY_CONST;
    ui->label_delay->setText(QString::number
                             (ui->horizontal_slider_delay->value()));
}

void MainWindow::on_timeout_game_time()
{
    seconds++;
    ui->label_time->setText(QString::fromStdString
                             (std::to_string(minutes) + ":"
                              + std::to_string(seconds)));
    if (seconds == 59)
    {
        minutes++;
        seconds = -1;
    }
}

void MainWindow::on_slider_change_fruits()
{
    ui->label_fruits->setText(QString::number
                              (ui->horizontal_slider_fruits->value()));
    number_of_fruits = ui->horizontal_slider_fruits->value() - 1;
    distr_ = std::uniform_int_distribution<int>(0, number_of_fruits);
    distr_(randomEng_);
}

