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

bool KeystoneScreen::glPaintOutput (
    const GLScreenPaintAttrib &attrib,
    const GLMatrix            &matrix,
    const CompRegion          &region,
    CompOutput                *output,
    unsigned int              mask
) {
    mask |= PAINT_SCREEN_TRANSFORMED_MASK |
            PAINT_SCREEN_WITH_TRANSFORMED_WINDOWS_MASK;

    GLMatrix sMatrix (matrix);
    if( optionGetKsEnable() )
    {
        const float ksh = optionGetKsHorz();
        const float ksv = optionGetKsVert();
//        sMatrix.rotate(ksh,0,0,1);
        sMatrix.scale(1+ksh,1+ksv,1.0);
    }
    bool status = glScreen->glPaintOutput(attrib, sMatrix, region, output, mask);
    return status;
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
        const float ksh = optionGetKsHorz();
        const float ksv = optionGetKsVert();
//        sMatrix.rotate(ksh,0,0,1);
        sMatrix.scale(1+ksh,1+ksv,1.0);
    }
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
//        cout << optionGetKsEnable() << ", " << optionGetKsHorz()  << ", " << optionGetKsVert() << endl;
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

#define setNotify(func) \
    optionSet##func##Notify (boost::bind (&KeystoneScreen::optionChanged, \
					  this, _1, _2))

    setNotify (KsEnable);
    setNotify (KsVert);
    setNotify (KsHorz);

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

