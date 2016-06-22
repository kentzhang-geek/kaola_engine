#include "mopenglview.h"
using namespace std;

#if 1

using namespace std;

MOpenGLView * one_view = NULL;

GLfloat points[][3] = {
    -1.0f, -1.0f, 0.0f, //0
    1.0f, 1.0f, 0.0f,  //2
    1.0f, -1.0f, 0.0f,  //1
    -1.0f, -1.0f,  0.0f,  //3
    1.0f, 1.0f,  0.0f,  //4
    -1.0f, 1.0f,  0.0f,  //5
};

typedef struct tag_point {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} m_point;

typedef struct tag_squre {
    m_point pts[6];
} m_squre;

const int cnt_squre = 18;

GLuint vbo = 0;
GLuint vao = 0;
GLuint ibo = 0;
GLuint program = 0;

void error_out(int err, const char* desc) {
    cout << "Error: " << err << " || " << desc << endl;
};


#endif

MOpenGLView::MOpenGLView()
{
    printf("qer\n");
}

void MOpenGLView::paintGL() {
    angle += 0.05f;
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//        glEnable(GL_POLYGON_SMOOTH);
//        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //ÊÓ½Ç¿í£¨½Ç¶È£©       ¿í¸ß±È    ½üÇÐÃæ  Ô¶ÇÐÃæ
    glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)this->size().width() / this->size().height(), 0.3f, 500.0f);
    //ÑÛ¾¦Î»ÖÃ      ¿´µÄµãµÄÎ»ÖÃ       Í·¶¥µÄÏòÁ¿
    glm::mat4 camera = glm::lookAt(glm::vec3(0, -1.6, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 1));
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.05));
    glm::mat4 pvm = proj * camera ;
    glm::mat4 unpv = glm::inverse(proj * camera);
    glUniformMatrix4fv(glGetUniformLocation(program, "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
    glUniformMatrix4fv(glGetUniformLocation(program, "m"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "unpv"), 1, GL_FALSE, glm::value_ptr(unpv));
    glUniform1f(glGetUniformLocation(program, "timein"), this->time_factor);
    this->time_factor += 0.5;

    glUseProgram(program);
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawArrays(GL_TRIANGLES, 0, 6 * 100 * 100);
}

void MOpenGLView::initializeGL() {
    QGLWidget::initializeGL();

    this->time_factor = 0;

    int w = 1024;
    int h = 768;

    glm::vec3 ptx = glm::vec3(0.4, 0.0, 2.0);

    glm::mat4 proje = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 500.0f);
    glm::mat4 viewm = glm::lookAt(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
    glm::vec4 ans = proje * viewm * glm::vec4(ptx, 1.0);

    char * vert_sl = "/Volumes/RamDisk/qt_test/qttest/vert.glsl";
    char * frag_sl = "/Volumes/RamDisk/qt_test/qttest/frag.glsl";

#define MAX_FILE_SIZE 10000
    char * filedata = new char[MAX_FILE_SIZE];
    bzero(filedata, MAX_FILE_SIZE);
    ifstream vsl = ifstream(vert_sl);
    vsl.read(filedata, MAX_FILE_SIZE);
    string vert(filedata);
    bzero(filedata, MAX_FILE_SIZE);
    ifstream fsl = ifstream(frag_sl);
    fsl.read(filedata, MAX_FILE_SIZE);
    string frag(filedata);

    const char* v_shader = vert.c_str();
    const char* f_shader = frag.c_str();

    const GLubyte* name = glGetString(GL_VENDOR);
    const GLubyte* biaoshifu = glGetString(GL_RENDERER);
    const GLubyte* OpenGLVersion = glGetString(GL_VERSION);    printf("OpenGL%s\n", name);
    printf("%s\n", biaoshifu);
    printf("OpenGL%s\n", OpenGLVersion);

    //create shaders
    /*GLuint vs = glCreateShader();*/
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &v_shader, nullptr);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &f_shader, nullptr);
    glCompileShader(fs);

    //shader error
    GLint *status = new GLint;
    glGetShaderiv(fs, GL_COMPILE_STATUS, status);
    if(*status == GL_FALSE){
        GLint length;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &length);
        string message("Failed to create ");
        message += (GL_FRAGMENT_SHADER == GL_VERTEX_SHADER ? "vertex shade:\n" : "fragment shader:\n");

        char *detail = new char[length];
        glGetShaderInfoLog(fs, length, &length, detail);
        message += detail;
        delete[] detail;

        glDeleteShader(fs);

        cout<<message<<endl;
    }

    //create program
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // set data
    m_squre * squrs = (m_squre *)malloc(sizeof(m_squre) * 100 * 100);
    for (int i = -49; i < 50; i++) {
        for (int j = -49; j < 50; j++) {
            for (int z = 0; z < 6; z++) {
                squrs[(i + 49) * 100 + j + 49].pts[z].x += points[z][0] + i;
                squrs[(i + 49) * 100 + j + 49].pts[z].y += points[z][1] + j;
            }
        }
    }

    //set vbo
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (100 * 100 * sizeof(m_squre)), squrs, GL_STATIC_DRAW);

    //set ibo
//    glGenBuffers(1, &ibo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexs), indexs, GL_STATIC_DRAW);

    //use vbo
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    angle = 0.0f;
}

MOpenGLView::MOpenGLView(QWidget *x) : QGLWidget(x) {
    auto f = this->format();
    f.setVersion(4, 1);
    f.setProfile(f.CoreProfile);
    this->setFormat(f);
    timer = new QTimer(this);
    timer->start(100);
    one_view = this;
//    this->connect(timer, SIGNAL(timeout()), this, SLOT(update()));
}

void MOpenGLView::setCon(bool s) {
    if (s) {
        this->connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    }
    else {
        this->disconnect(timer, SIGNAL(timeout()), this, SLOT(update()));
    }
}
