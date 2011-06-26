#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <GL/glu.h>
#include <dlfcn.h>

#include <core/atoms.h>

#include "keystone.h"

#define PI 3.14159265359f

using namespace std;
#include <iostream>

COMPIZ_PLUGIN_20090315 (keystone, KeystonePluginVTable);

void KeystoneScreen::releaseMoveWindow ()
{
}

void KeystoneScreen::handleEvent (XEvent *event)
{
    switch (event->type) {
    case ClientMessage:
//	if (event->xclient.message_type == Atoms::desktopViewport) {}
//	else if (event->xclient.message_type == Atoms::xdndEnter) {}
//	else if (event->xclient.message_type == Atoms::xdndLeave) {}
	break;

	case FocusIn:
	case FocusOut:
//	    if (event->xfocus.mode == NotifyGrab) poller.start ();
//	    else if (event->xfocus.mode == NotifyUngrab) poller.stop ();
	break;

	case ConfigureNotify:
//	     if (event->xconfigure.window == screen->root ())
//		updateScreenEdgeRegions ();

	break;
    }

    screen->handleEvent (event);
}

void KeystoneWindow::activate ()
{
    window->activate ();
}

void KeystoneWindow::grabNotify (int x, int y, unsigned int width, unsigned int height)
{
    window->grabNotify (x, y, width, height);
}

void KeystoneWindow::ungrabNotify ()
{
    window->ungrabNotify ();
}

void KeystoneScreen::preparePaint (int msSinceLastPaint)
{
    cScreen->preparePaint (msSinceLastPaint);
}

void KeystoneScreen::paint (CompOutput::ptrList& outputs, unsigned int mask)
{
    cScreen->paint (outputs, mask);
}

void KeystoneScreen::donePaint()
{
    cScreen->donePaint ();
}

inline void vert(float* m, float a)
{
    m[0] = 1;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;

    m[4] = 0;
    m[5] = cos(a);
    m[6] = -sin(a);
    m[7] = 0;

    m[8] = 0;
    m[9] = sin(a);
    m[10] = cos(a);
    m[11] = 0;

    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;
}

inline void horiz(float* m, float a)
{
    m[0] = cos(a);
    m[1] = 0;
    m[2] = sin(a);
    m[3] = 0;

    m[4] = 0;
    m[5] = 1;
    m[6] = 0;
    m[7] = 0;

    m[8] = -sin(a);
    m[9] = 0;
    m[10] = cos(a);
    m[11] = 0;

    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;

}

bool KeystoneScreen::glPaintOutput (
    const GLScreenPaintAttrib &attrib,
    const GLMatrix            &matrix,
    const CompRegion          &region,
    CompOutput                *output,
    unsigned int              mask
) {
    if( optionGetKsEnable() )
    {
        mask |= PAINT_SCREEN_TRANSFORMED_MASK |
                PAINT_SCREEN_WITH_TRANSFORMED_WINDOWS_MASK;
    }

    return glScreen->glPaintOutput(attrib, matrix, region, output, mask);
}

void KeystoneScreen::glPaintTransformedOutput (
    const GLScreenPaintAttrib &attrib,
    const GLMatrix            &matrix,
    const CompRegion          &region,
    CompOutput                *output,
    unsigned int              mask
) {
    if (mask & PAINT_SCREEN_CLEAR_MASK)
        glScreen->clearTargetOutput (GL_COLOR_BUFFER_BIT);
    mask &= ~PAINT_SCREEN_CLEAR_MASK;

    GLMatrix sMatrix (matrix);
    if( optionGetKsEnable() )
    {
        const float rh = optionGetKsHorz();
        const float rv = optionGetKsVert();
        const float ph = optionGetKsPosx();
        const float pv = optionGetKsPosy();
        const float s = optionGetKsScale();
        float m[4*4];
        horiz(m,rh);
        GLMatrix mhr(m);
        vert(m,rv);
        GLMatrix mvr(m);

//        sMatrix = mvr * mhr * matrix;
        sMatrix.rotate(-100*rh,0,1,0);
        sMatrix.rotate(100*rv,1,0,0);
        sMatrix.translate(ph,pv,s);
    }
    glScreen->clearTargetOutput (GL_COLOR_BUFFER_BIT);

    glScreen->glPaintTransformedOutput(attrib, sMatrix, region, output, mask);
}

bool KeystoneWindow::glPaint (
    const GLWindowPaintAttrib &attrib,
    const GLMatrix            &matrix,
    const CompRegion          &region,
    unsigned int              mask
) {
    return glWindow->glPaint (attrib, matrix, region, mask);
}

void KeystoneScreen::optionChanged (CompOption           *opt,
			   KeystoneOptions::Options num)
{
    switch(num) {
    case KeystoneOptions::KsEnable:
    case KeystoneOptions::KsVert:
    case KeystoneOptions::KsHorz:
        cScreen->damageScreen();
        break;

    default:
	break;
    }
}

bool KeystoneScreen::setOptionForPlugin(
    const char        *plugin,
    const char        *name,
    CompOption::Value &value)
{
    bool status = screen->setOptionForPlugin (plugin, name, value);

//    if (strcmp (plugin, "core") == 0)
//    {
//    }

    return status;
}

void KeystoneScreen::matchExpHandlerChanged ()
{
    screen->matchExpHandlerChanged ();
}

void KeystoneScreen::matchPropertyChanged (CompWindow *window)
{
    screen->matchPropertyChanged (window);
}

bool KeystoneScreen::ToggleViewportEnableDisable()
{
    const bool state = optionGetKsEnable();
    CompOption::Value v(!state);
    getOptions()[KsEnable].set(v);
    cScreen->damageScreen();
    return true;
}

bool KeystoneScreen::AdjustKeystone(float hinc, float vinc)
{
    const float h = optionGetKsHorz() + hinc;
    const float v = optionGetKsVert() + vinc;
    CompOption::Value vh(h);
    CompOption::Value vv(v);
    getOptions()[KsHorz].set(vh);
    getOptions()[KsVert].set(vv);
    return true;
}

bool KeystoneScreen::AdjustPos(float hinc, float vinc)
{
    const float h = optionGetKsPosx() + hinc;
    const float v = optionGetKsPosy() + vinc;
    CompOption::Value vh(h);
    CompOption::Value vv(v);
    getOptions()[KsPosx].set(vh);
    getOptions()[KsPosy].set(vv);
    return true;
}

bool KeystoneScreen::AdjustScale(float sinc)
{
    const float s = optionGetKsScale() + sinc;
    CompOption::Value v(s);
    getOptions()[KsScale].set(v);
    return true;
}

KeystoneScreen::KeystoneScreen (CompScreen *screen) :
    PluginClassHandler <KeystoneScreen, CompScreen> (screen),
    KeystoneOptions (),
    cScreen (CompositeScreen::get (screen)),
    glScreen (GLScreen::get (screen))
{
    ScreenInterface::setHandler (screen);
    CompositeScreenInterface::setHandler (cScreen);
    GLScreenInterface::setHandler (glScreen);

    const float inc = 0.01;

    optionSetKsEnableKeyInitiate(boost::bind(&KeystoneScreen::ToggleViewportEnableDisable, this));
    optionSetLeftKeyInitiate(    boost::bind(&KeystoneScreen::AdjustKeystone, this, -inc, 0));
    optionSetRightKeyInitiate(   boost::bind(&KeystoneScreen::AdjustKeystone, this, +inc, 0));
    optionSetUpKeyInitiate(      boost::bind(&KeystoneScreen::AdjustKeystone, this, 0, +inc));
    optionSetDownKeyInitiate(    boost::bind(&KeystoneScreen::AdjustKeystone, this, 0, -inc));

    optionSetLeftxKeyInitiate(   boost::bind(&KeystoneScreen::AdjustPos, this, -inc, 0));
    optionSetRightxKeyInitiate(  boost::bind(&KeystoneScreen::AdjustPos, this, +inc, 0));
    optionSetUpxKeyInitiate(     boost::bind(&KeystoneScreen::AdjustPos, this, 0, +inc));
    optionSetDownxKeyInitiate(   boost::bind(&KeystoneScreen::AdjustPos, this, 0, -inc));

    optionSetDownsKeyInitiate(   boost::bind(&KeystoneScreen::AdjustScale, this, -inc));
    optionSetUpsKeyInitiate(     boost::bind(&KeystoneScreen::AdjustScale, this, +inc));

#define setNotify(func) \
    optionSet##func##Notify (boost::bind (&KeystoneScreen::optionChanged, \
					  this, _1, _2))

    setNotify (KsEnable);
    setNotify (KsVert);
    setNotify (KsHorz);
    setNotify (KsPosx);
    setNotify (KsPosy);
    setNotify (KsScale);

//    poller.setCallback (boost::bind (&KeystoneScreen::positionUpdate, this, _1));
}

KeystoneScreen::~KeystoneScreen ()
{
}

KeystoneWindow::KeystoneWindow (CompWindow *window) :
    PluginClassHandler <KeystoneWindow, CompWindow> (window),
    window (window),
    glWindow (GLWindow::get (window))
{
    GLWindowInterface::setHandler (glWindow);
    WindowInterface::setHandler (window);
}

bool
KeystonePluginVTable::init ()
{
    if (!CompPlugin::checkPluginABI ("core", CORE_ABIVERSION))
        return false;
    if (!CompPlugin::checkPluginABI ("composite", COMPIZ_COMPOSITE_ABI))
	return false;
    if (!CompPlugin::checkPluginABI ("opengl", COMPIZ_OPENGL_ABI))
	return false;
    if (!CompPlugin::checkPluginABI ("mousepoll", COMPIZ_MOUSEPOLL_ABI))
	return false;

    return true;
}

