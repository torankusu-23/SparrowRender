#pragma once
#include "cmath"
#include "iostream"
#include "constant.h"
#include "Image.h"


struct vec2 {//目测巨量bug待改，但更目测大量bug根本跑不到,函数可能要删除
	float p[2];
	/*
	vec2() : p{0, 0} {}
	vec2(float x, float y) : p{x, y} {}
	
	inline float x() const { return p[0]; }
	inline float y() const { return p[1]; }
	inline float operator[](int i) const { return p[i]; }	//不返回引用就无法修改
	inline float& operator[](int i) { return p[i]; }//返回引用可能导致该值被修改，或者vec对象失效后，指向空数据，需要保留两种函数
	inline vec2 operator+(const vec2& n) const { return vec2(p[0] + n.p[0], p[1] + n.p[1]); }
	inline vec2 operator-(const vec2& n) const { return vec2(p[0] - n.p[0], p[1] - n.p[1]); }
	inline vec2 operator*(float n) const { return vec2((float)p[0] * n, (float)p[1] * n); }
	inline vec2 operator/(float n) const { return *this * (1 / n); }
	inline vec2 operator*(const vec2& v) {return vec2(p[0] * v.p[0], p[1] * v.p[1]);}
	inline vec2 normaliz() { return *this / (sqrt(p[0] * p[0] + p[1] * p[1])); }
	*/
	vec2();
	vec2(float x, float y);
	float x() const;
	float y() const;
	float operator[](int i) const;
	float& operator[](int i);
	vec2 operator+(const vec2& n) const;
	vec2 operator-(const vec2& n) const;
	vec2 operator*(float n) const;
	vec2 operator/(float n) const;
	vec2 operator*(const vec2& v);
	vec2 normaliz();
};
vec2 operator*(float n, vec2& u);
std::ostream& operator<<(std::ostream& out, const vec2& v);




struct vec3 {	//重复性较大，可以和vec2合并优化，但融合的数据结构比较难看明白
	float p[3];
	/*
	vec3() :p{0, 0, 0} {};
	vec3(float x, float y, float z): p{ x, y, z } {}
	
	inline float x() const { return p[0]; }
	inline float y() const { return p[1]; }
	inline float z() const { return p[2]; }
	inline float operator[](int i) const { return p[i]; }	
	inline float& operator[](int i) { return p[i]; }
	inline vec3 operator+(const vec3& n) const { return vec3(p[0] + n.p[0], p[1] + n.p[1], p[2] + n.p[2]); }//大量用不到fun的可以删除
	inline vec3 operator-(const vec3& n) const { return vec3(p[0]-n.p[0], p[1] - n.p[1], p[2] - n.p[2]); }
	inline vec3 operator*(float n) const { return vec3(p[0] * n, p[1] * n, p[2] * n); }
	inline vec3 operator/(float n) const { return *this * (1 / n); }
	inline vec3 operator*(const vec3& v) { return vec3(p[0] * v.p[0], p[1] * v.p[1], p[2] * v.p[2]); }
	inline float norm() { return sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]); }	//膜长
	inline vec3 normalize() { return *this / this->norm();}	//归一化
	*/
	vec3();
	vec3(float x, float y, float z);
	float x() const;
	float y() const;
	float z() const;
	float operator[](int i) const;
	float& operator[](int i);
	vec3 operator+(const vec3& n) const;
	vec3 operator-(const vec3& n) const;
	vec3 operator*(float n) const;
	vec3 operator/(float n) const;
	vec3 operator*(const vec3& v);
	float norm();
	vec3 normalize();
};
vec3 operator*(float t, const vec3& v);
std::ostream& operator<<(std::ostream& out, const vec3& v);
float dot(vec3& u, vec3& v);
vec3 cross(const vec3& u, const vec3& v);



struct vec4 {	//可以和vec2合并优化，但这样的数据结构比较难看清
	float p[4];

	vec4() :p{ 0, 0, 0, 0 } {};
	vec4(float x, float y, float z, float w) : p{ x, y, z, w } {}
	inline float x() const { return p[0]; }
	inline float y() const { return p[1]; }
	inline float z() const { return p[2]; }
	inline float w() const { return p[3]; }
	inline float operator[](int i) const { return p[i]; }
	inline float& operator[](int i) { return p[i]; }
	inline vec4 operator+(const vec4& n) const { return vec4(p[0] + n.p[0], p[1] + n.p[1], p[2] + n.p[2], p[3] + n.p[3]); }//大量用不到fun的可以删除
	inline vec4 operator-(const vec4& n) const { return vec4(p[0] - n.p[0], p[1] - n.p[1], p[2] - n.p[2], p[3] - n.p[3]); }
	inline vec4 operator*(float n) const { return vec4(p[0] * n, p[1] * n, p[2] * n, p[3] * n); }
	inline vec4 operator/(float n) const { return *this * (1 / n); }
	inline vec4 operator*(const vec4& v) { return vec4(p[0] * v.p[0], p[1] * v.p[1], p[2] * v.p[2], p[3] * v.p[3]); }
	
};
vec4 operator*(float t, const vec4& v);	//换序
std::ostream& operator<<(std::ostream& out, const vec4& v);
vec4 up_to_vec4(const vec3& u, float w);
vec3 down_to_vec3(const vec4& u);



struct mat3 
{
	vec3 rows[3];
	
	inline vec3& operator[](int i) { return rows[i]; }
	inline vec3 operator[](int i) const { return rows[i]; }	
	mat3 transpose() const;		//转置矩阵
	mat3 inverse() const;		//逆矩阵
	inline void identity() { rows[0][0] = 1; rows[1][1] = 1; rows[2][2] = 1;}
};
std::ostream& operator<<(std::ostream& out, const mat3& m);



struct mat4
{
	vec4 rows[4];

	inline vec4& operator[](int i) { return rows[i]; }
	inline vec4 operator[](int i) const { return rows[i]; }
	mat4 operator*(const mat4& mat);

	mat4 transpose() const;		//转置矩阵
	mat4 inverse() const;		//逆矩阵
	inline void identity() {rows[0][0] = 1; rows[1][1] = 1; rows[2][2] = 1; rows[3][3] = 1;}

};
std::ostream& operator<<(std::ostream& out, const mat4& m);
vec4 operator*(const mat4& m, const vec4 v);



