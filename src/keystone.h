/**
 *
 * Compiz keystone plugin
 *
 * keystone.h
 *
 * Copyright (c) 2006 Robert Carr <racarr@beryl-project.org>
 *
 * Authors:
 * Robert Carr <racarr@beryl-project.org>
 * Dennis Kasprzyk <onestone@opencompositing.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 **/

#include <core/core.h>
#include <core/pluginclasshandler.h>
#include <composite/composite.h>
#include <opengl/opengl.h>
#include <mousepoll/mousepoll.h>

#include "keystone_options.h"

class KeystoneScreen :
    public ScreenInterface,
    public CompositeScreenInterface,
    public GLScreenInterface,
    public PluginClassHandler <KeystoneScreen, CompScreen>,
    public KeystoneOptions
{
    public:
	KeystoneScreen (CompScreen *s);
	~KeystoneScreen ();

	void preparePaint (int);
	void paint (CompOutput::ptrList &, unsigned int);
	void donePaint ();
	void handleEvent (XEvent *event);

	bool glPaintOutput (const GLScreenPaintAttrib &,
			    const GLMatrix &, const CompRegion &,
			    CompOutput *, unsigned int);
	void glPaintTransformedOutput (const GLScreenPaintAttrib &,
				       const GLMatrix &,
				       const CompRegion &,
				       CompOutput *, unsigned int);

	bool setOptionForPlugin (const char *, const char *,
				 CompOption::Value&);
	void matchExpHandlerChanged ();
	void matchPropertyChanged (CompWindow *);

	void releaseMoveWindow ();

	void optionChanged (CompOption *opt, KeystoneOptions::Options num);

        bool ToggleViewportEnableDisable();
        bool AdjustKeystone(float h, float v);

	CompositeScreen *cScreen;
	GLScreen        *glScreen;

//	MousePoller	 poller;
};

class KeystoneWindow :
	public WindowInterface,
	public GLWindowInterface,
	public PluginClassHandler <KeystoneWindow, CompWindow>
{
    public:
	KeystoneWindow (CompWindow *);

	virtual void activate ();
	void grabNotify (int, int, unsigned int, unsigned int);
	void ungrabNotify ();
	bool glPaint (const GLWindowPaintAttrib &, const GLMatrix &,
		      const CompRegion &, unsigned int);

	CompWindow *window;
	GLWindow   *glWindow;
};

class KeystonePluginVTable :
    public CompPlugin::VTableForScreenAndWindow <KeystoneScreen, KeystoneWindow>
{
    public:

	bool init ();
};

