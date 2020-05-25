#include <QApplication>
#include <QTranslator>
#include <QIcon>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setApplicationName("otpgen");
    a.setApplicationDisplayName("OTPGen");
    a.setWindowIcon(QIcon(":/app-icon.svgz"));

    // load embedded translations for current locale using QRC language and alias magic :)
    // falls back to embedded English strings if no translation was found
    QTranslator translator;
    if (translator.load(QLocale(), ":/i18n/lang.qm"))
    {
        a.installTranslator(&translator);
    }

    return 0;
}
