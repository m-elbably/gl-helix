#include <windows.h>
#include <iostream>
#include <gl\gl.h>
#include <gl\glu.h>

#ifndef __ARB_MULTISAMPLE_H__
#define __ARB_MULTISAMPLE_H__

extern bool arbMultisampleSupported;
extern int arbMultisampleFormat;

#define CHECK_FOR_MULTISAMPLE 1
int LoadGLTextures();
#endif

HDC hDC = NULL;
HGLRC hRC = NULL;
HWND hWnd = NULL;
HINSTANCE hInstance;

bool keys[256];
bool active = TRUE;
bool fullscreen = TRUE;

GLuint base;
GLuint texture[2];

float no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
float mat_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
float mat_ambient_color[] = {0.8f, 0.8f, 0.2f, 1.0f};
float mat_diffuse[] = {0.1f, 0.6f, 1.0f, 1.0f};
float mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
float no_shininess = 0.0f;
float low_shininess = 5.0f;
float high_shininess = 10.0f;
float mat_emission[] = {0.3f, 0.2f, 0.2f, 1.0f};

int filter;
int fogMode[] = {GL_EXP, GL_EXP2, GL_LINEAR};
int fogfilter = 0;
float fogColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

GLUquadric* gqd = gluNewQuadric();
int mn = -16;
int rv = 0;
bool pass = false;

int rot = 0;

int rev = 0;
bool revOn;

struct Polymer {
  float rotation;
  float maxRot;

  void Rotate(float value) {
    // if(value>360)
    //	rotation = value - 360;
    // else
    rotation = value;
  }

  void Draw(int column) {
    glPushMatrix();
    glRotatef(rotation, 1, 0, 0);

    glPushMatrix();
    glTranslatef(column * 0.04, -0.1, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(gqd, .0050, 0.0050, 0.2, 16, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(column * 0.04, -0.1, 0);
    glColor4ub(180, 180, 40, 180);
    gluSphere(gqd, 0.02, 32, 32);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(column * 0.04, 0.1, 0);
    glColor4ub(180, 180, 40, 180);
    gluSphere(gqd, 0.02, 32, 32);
    glPopMatrix();

    glPopMatrix();
  }
};

Polymer dna[32];
int currentPoly;
int phase;
int roll;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
GLvoid ReSizeGLScene(GLsizei width, GLsizei height) {
  if (height == 0) {
    height = 1;
  }

  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

int InitGL(GLvoid) {
  glShadeModel(GL_SMOOTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  return TRUE;
}

void ResetPolymers() {
  for (int i = 0; i < 32; i++) {
    dna[i].rotation = 0;
  }
}

int DrawGLScene(GLvoid) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0, 0, -1.4);

  glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, high_shininess);
  glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

  glLightModeli(GL_LIGHT_MODEL_AMBIENT, 1);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  glFogi(GL_FOG_MODE, fogMode[2]);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.35f);
  glHint(GL_FOG_HINT, GL_DONT_CARE);
  glFogf(GL_FOG_START, 1.3f);
  glFogf(GL_FOG_END, 1.5f);
  glEnable(GL_FOG);

  glPushMatrix();

  for (int i = -16; i < 16; i++) {
    if (phase == 0) {
      if (currentPoly < 32) {
        if (i + 16 == currentPoly) {
          dna[currentPoly].Rotate(dna[currentPoly].rotation + 1);

          if (dna[currentPoly].rotation >= (i + 17) * 14) currentPoly++;
        } else if (i + 16 > currentPoly) {
          dna[i + 16].Rotate(dna[i + 16].rotation + 1);
        }
      } else
        phase = 1;
    } else if (phase == 1) {
      if (roll < 2880 * 14) {
        dna[i + 16].Rotate(dna[i + 16].rotation + 1);
        roll++;
      } else {
        phase = 2;
        currentPoly = 0;
      }
    } else if (phase == 2) {
      if (currentPoly < 32) {
        if (i + 16 >= currentPoly) {
          dna[i + 16].Rotate(dna[i + 16].rotation + 1);
        }

        if ((int)(dna[currentPoly].rotation * 2) % 360 == 0) currentPoly++;
      } else {
        ResetPolymers();
        phase = 0;
        currentPoly = 0;
      }
    }

    dna[i + 16].Draw(i);
  }

  glPopMatrix();

  return TRUE;
}

GLvoid KillGLWindow(GLvoid) {
  if (fullscreen) {
    ChangeDisplaySettings(NULL, 0);
    ShowCursor(TRUE);
  }

  if (hRC) {
    if (!wglMakeCurrent(NULL, NULL)) {
      MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR",
                 MB_OK | MB_ICONINFORMATION);
    }

    if (!wglDeleteContext(hRC)) {
      MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR",
                 MB_OK | MB_ICONINFORMATION);
    }
    hRC = NULL;
  }

  if (hDC && !ReleaseDC(hWnd, hDC)) {
    MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR",
               MB_OK | MB_ICONINFORMATION);
    hDC = NULL;
  }

  if (hWnd && !DestroyWindow(hWnd)) {
    MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR",
               MB_OK | MB_ICONINFORMATION);
    hWnd = NULL;
  }

  if (!UnregisterClass("OpenGL", hInstance)) {
    MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR",
               MB_OK | MB_ICONINFORMATION);
    hInstance = NULL;
  }
}

BOOL CreateGLWindow(char* title, int width, int height, int bits,
                    bool fullscreenflag) {
  GLuint PixelFormat;
  WNDCLASS wc;
  DWORD dwExStyle;
  DWORD dwStyle;
  RECT WindowRect;
  WindowRect.left = (long)0;
  WindowRect.right = (long)width;
  WindowRect.top = (long)0;
  WindowRect.bottom = (long)height;

  fullscreen = fullscreenflag;

  hInstance = GetModuleHandle(NULL);
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc = (WNDPROC)WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "GLH";

  if (!RegisterClass(&wc)) {
    MessageBox(NULL, "Failed To Register The Window Class.", "ERROR",
               MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  if (fullscreen) {
    DEVMODE dmScreenSettings;
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth = width;
    dmScreenSettings.dmPelsHeight = height;
    dmScreenSettings.dmBitsPerPel = bits;
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) !=
        DISP_CHANGE_SUCCESSFUL) {
      if (MessageBox(NULL,
                     "The Requested Fullscreen Mode Is Not Supported By\nYour "
                     "Video Card. Use Windowed Mode Instead?",
                     "Helix", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
        fullscreen = FALSE;
      } else {
        MessageBox(NULL, "Program Will Now Close.", "ERROR",
                   MB_OK | MB_ICONSTOP);
        return FALSE;
      }
    }
  }

  if (fullscreen) {
    dwExStyle = WS_EX_APPWINDOW;
    dwStyle = WS_POPUP;
    ShowCursor(FALSE);
  } else {
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    dwStyle = WS_OVERLAPPEDWINDOW;
  }

  AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);
  if (!(hWnd = CreateWindowEx(dwExStyle, "GLH", title,
                              dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0,
                              WindowRect.right - WindowRect.left,
                              WindowRect.bottom - WindowRect.top, NULL, NULL,
                              hInstance, NULL))) {
    KillGLWindow();
    MessageBox(NULL, "Window Creation Error.", "ERROR",
               MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  static PIXELFORMATDESCRIPTOR pfd =

                                   sizeof(PIXELFORMATDESCRIPTOR),
                               1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                                      PFD_DOUBLEBUFFER,
                               PFD_TYPE_RGBA, bits, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
};

if (!(hDC = GetDC(hWnd))) {
  KillGLWindow();
  MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR",
             MB_OK | MB_ICONEXCLAMATION);
  return FALSE;
}

if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) {
  KillGLWindow();
  MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR",
             MB_OK | MB_ICONEXCLAMATION);
  return FALSE;
}

if (!SetPixelFormat(hDC, PixelFormat, &pfd)) {
  KillGLWindow();
  MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR",
             MB_OK | MB_ICONEXCLAMATION);
  return FALSE;
}

if (!(hRC = wglCreateContext(hDC))) {
  KillGLWindow();
  MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR",
             MB_OK | MB_ICONEXCLAMATION);
  return FALSE;
}

if (!wglMakeCurrent(hDC, hRC)) {
  KillGLWindow();
  MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR",
             MB_OK | MB_ICONEXCLAMATION);
  return FALSE;
}

ShowWindow(hWnd, SW_SHOW);
SetForegroundWindow(hWnd);
SetFocus(hWnd);
ReSizeGLScene(width, height);

if (!InitGL()) {
  KillGLWindow();
  MessageBox(NULL, "Initialization Failed.", "ERROR",
             MB_OK | MB_ICONEXCLAMATION);
  return FALSE;
}

return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_ACTIVATE: {
      if (!HIWORD(wParam)) {
        active = TRUE;
      } else {
        active = FALSE;
      }

      return 0;
    }

    case WM_SYSCOMMAND: {
      switch (wParam) {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
          return 0;
      }
      break;
    }

    case WM_CLOSE: {
      PostQuitMessage(0);
      return 0;
    }

    case WM_KEYDOWN: {
      keys[wParam] = TRUE;
      return 0;
    }

    case WM_KEYUP: {
      keys[wParam] = FALSE;
      return 0;
    }

    case WM_SIZE: {
      ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
      return 0;
    }
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  std::string cmd = GetCommandLine();

  if (cmd.find("/c", 0) != std::string::npos) {
    MessageBox(hWnd, "This screen saver has no settings", "Helix", 0);
    return 0;
  }

  else if (cmd.find("/p", 0) != std::string::npos) {
    return 0;
  }

  MSG msg;
  BOOL done = FALSE;

  fullscreen = FALSE;

  if (!CreateGLWindow("helix", GetSystemMetrics(SM_CXSCREEN),
                      GetSystemMetrics(SM_CYSCREEN), 32, TRUE)) {
    return 0;
  }

  while (!done) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) {
        done = TRUE;
      } else {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    } else {
      if (active) {
        if (keys[VK_ESCAPE]) {
          done = TRUE;
        } else {
          DrawGLScene();
          SwapBuffers(hDC);
        }
      }
    }
  }

  KillGLWindow();
  return (msg.wParam);
}