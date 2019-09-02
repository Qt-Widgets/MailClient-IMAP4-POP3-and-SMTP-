#include "IndicatorBar.h"
#include "ThemeHandler.h"

IndicatorBar::IndicatorBar()
{
    movie.setFileName(":images/barprogress.gif");
    setMovie(&movie);
    setVisible(false);
}

void IndicatorBar::Start()
{
    setVisible(true);
    show();
    movie.start();}

void IndicatorBar::Stop()
{
    setVisible(false);
    hide();
    movie.stop();
}
