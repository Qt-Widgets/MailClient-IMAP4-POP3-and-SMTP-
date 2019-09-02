#include "IndicatorCircular.h"
#include "ThemeHandler.h"

IndicatorCircular::IndicatorCircular()
{
    movie.setFileName(":images/cirularprogress.gif");
    setMovie(&movie);
    setVisible(false);
}

void IndicatorCircular::Start()
{
    setVisible(true);
    show();
    movie.start();
}

void IndicatorCircular::Stop()
{
    setVisible(false);
    hide();
    movie.stop();
}
