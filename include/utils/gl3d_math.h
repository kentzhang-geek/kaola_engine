#ifndef GL3D_MATH_H
#define GL3D_MATH_H

#include "kaola_engine/gl3d_out_headers.h"
#include "utils/gl3d_utils.h"
#include <QVector>

namespace gl3d {
    namespace math {
        template<class T>
        class line {
        public:
            T a;
            T b;

            line() : a(T(0.0f)), b(T(0.0f)) {}

            line(T tag_a, T tag_b) : a(tag_a), b(tag_b) {}

            line(const line<T> &l) : a(l.a), b(l.b) {}

            line(const line<T> *l) : a(l->a), b(l->b) {}

            void swap_ab() {
                T tmp = this->b;
                this->b = this->a;
                this->a = tmp;
            }

            float length() { return glm::length(b - a); }

            bool point_on_line(T pt) {
                return (glm::length(this->a - pt) + glm::length(this->b - pt)) <=
                       (glm::length(this->a - this->b) + 0.0001);
            }

            float point_min_distance_to_vertex(T pt) {
                return glm::min(glm::length(pt - this->a), glm::length(pt - this->b));
            }
        };

        typedef line<glm::vec2> line_2d;
        typedef line<glm::vec3> line_3d;

        template<typename T>
        bool equal(T v1, T v2) {
            return (glm::length(v1 - v2) < 0.0001);
        }

        class triangle_facet {
        public:

            glm::vec3 a;
            glm::vec3 b;
            glm::vec3 c;

            bool is_valid_facet() const;

            bool is_point_in_facet(glm::vec3 pt) const;

            triangle_facet();

            triangle_facet(const triangle_facet &f);

            triangle_facet(glm::vec3 ta, glm::vec3 tb, glm::vec3 tc);

            glm::vec3 get_normal() const;

        };

        bool get_cross(const line_2d &l1, const line_2d &l2, glm::vec2 &cross_point);

        float point_distance_to_line(const glm::vec3 pt, const line_3d l);

        float point_distance_to_line(const glm::vec2 pt, const line_2d l);

        float point_distance_to_facet(const triangle_facet &fc, const glm::vec3 &pt);

        bool point_project_to_line(const line_2d &l, const glm::vec2 &pt, glm::vec2 &out_pt);

        bool line_cross_facet(const triangle_facet &f, const line_3d &ray, glm::vec3 &pt);

        void math_local_sort_line(QVector<line_2d> &lines);

        QVector<glm::vec3> generate_area(QVector<line_2d> &lines);

        glm::mat4 get_rotation_from_a_to_b(glm::vec3 a, glm::vec3 b);

        glm::vec3 convert_vec2_to_vec3(glm::vec2 a);

        template <typename T>
        void get_bounding(QVector<T> tss, T & t_max, T & t_min) {
            t_max = tss.at(0);
            t_min = tss.at(0);
            Q_FOREACH(T tit, tss) {
                    for (int i = 0; i < tit.length(); i++) {
                        t_max[i] = (tit[i] > t_max[i]) ? tit[i] : t_max[i];
                        t_min[i] = (tit[i] < t_min[i]) ? tit[i] : t_min[i];
                    }
                }
        }

        template <typename T>
        T get_left_top_vertex(QVector<T> tss) {
            T t_ret = tss.at(0);
            Q_FOREACH(T tit, tss) {
                    if (tit.length() == 2) {
                        if ((tit[1] < t_ret[1]) || ((tit[1] == t_ret[1]) && (tit[0] >= t_ret[0])))
                            t_ret = tit;
                    }
                    if (tit.length() == 3) {
                        if ((tit[2] > t_ret[2]) || ((tit[2] == t_ret[2]) && (tit[0] >= t_ret[0])))
                            t_ret = tit;
                    }
                }

            return t_ret;
        }

        template <typename T>
        T get_center_vertex(QVector<T> tss) {
            T t_ret(0.0f);
            Q_FOREACH(T tit, tss) {
                    t_ret += tit;
                }
            t_ret = t_ret / tss.size();

            return t_ret;
        }

        template <typename T>
        bool has_near_point_in_vector(QVector<T> pts, T pt) {
            Q_FOREACH(T it, pts) {
                    if (glm::length(it - pt) < 0.001)
                        return true;
                }
            return false;
        }

        template  <typename T>
        bool point_near_point(T pa, T pb) {
            return glm::length(pa - pb) < 0.001f;
        }

        template <typename  T>
        void sort_vector_by_distance(QVector<T> & vts, T origin) {
            QVector<T> tmp;
            tmp.clear();

            Q_FOREACH(T tit, vts) {
                    tmp.push_back(tit);
                }
            vts.clear();

            float dis = glm::length(origin - tmp.at(0));
            int pos = 0;
            while (tmp.size() > 0) {
                dis = glm::length(origin - tmp.at(0));
                pos = 0;
                for (int i = 0; i < tmp.size(); i++) {
                    if (dis > glm::length(origin - tmp.at(i))) {
                        dis = glm::length(origin - tmp.at(i));
                        pos = i;
                    }
                }

                vts.push_back(tmp.at(pos));
                tmp.remove(pos);
            }

            return;
        }
    }
}

#endif // GL3D_MATH_H
