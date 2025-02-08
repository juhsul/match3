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

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
