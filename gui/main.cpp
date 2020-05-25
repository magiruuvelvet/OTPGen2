#include <QApplication>
#include <QTranslator>
#include <QIcon>

#include <memory>
#include <cstdint>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setApplicationName("otpgen");
    a.setApplicationDisplayName("OTPGen");
    a.setWindowIcon(QIcon(":/app-icon.svgz"));

    // load embedded translations for current locale using QRC language and alias magic :)
    // falls back to embedded English strings if no translation was found
    std::unique_ptr<QTranslator> translator = std::make_unique<QTranslator>();
    if (translator->load(QLocale(), ":/i18n/lang.qm"))
    {
        a.installTranslator(translator.get());
        a.setProperty("translator", QVariant::fromValue(reinterpret_cast<std::uintptr_t>(translator.get())));
    }
    else
    {
        translator.reset();
        a.setProperty("translator", QVariant());
    }

    return 0;
}
