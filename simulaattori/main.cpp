#include <mikroplot/window.h>
#include <glm/glm.hpp>
#include <time.h>
#include <complex>
#include <glm/vec2.hpp>


/// Functions for integrion
namespace integrate {
	template<typename V>
	V add(const V& a, const V& b) {
		V res = a;
		for(size_t i=0; i<res.size(); ++i){
			res[i] = a[i] + b[i];
		}
		return res;
	}

	template<typename S, typename V>
	V mul(const S& a, const V& b) {
		V res = b;
		for(size_t i=0; i<res.size(); ++i){
			res[i] = a * b[i];
		}
		return res;
	}

	template<typename V>
	V add3(const V& a, const V& b) {
		V res = a;
	//	std::vector<glm::vec3> nums = {b};
		for (size_t i = 0; i < 3; ++i) {
			res[i] = a[i] + b[i];
		}
		return res;
	}

	template<typename S, typename V>
	V mul3(const S& a, const V& b) {
		V res = b;
	//	std::vector<glm::vec3> nums = { b };
		for (size_t i = 0; i < 3; ++i) {
			res[i] = a * b[i];
		}
		return res;
	}


	template<>
	glm::vec2 add(const glm::vec2& a, const glm::vec2& b) {
		return a + b;
	}

	template<>
	glm::vec2 mul(const float& a, const glm::vec2& b) {
		return a * b;
	}

	template<typename S, typename V, typename F>
	auto euler(const V& x0, S dt, F derive) {
		// Kysy paljonko f'(0) on
		auto dx = derive(x0, 0);
		// Laske f(0)+x*f'(x)
		return add(x0, mul(dt, dx));
	}

	template<typename S, typename V, typename F>
	auto euler3(const V& x0, S dt, F derive) {
		// Kysy paljonko f'(0) on
		auto dx = derive(x0, 0);
		// Laske f(0)+x*f'(x)
		return add3(x0, mul3(dt, dx));
	}

	template<typename S, typename V, typename F>
	auto midPoint(const V& x0, S dt, F derive) {
		// Tutki Eulerin menetelmän avulla paljonko on f(x/2)
		// Kysy paljonko on f'(0)
		auto dx0 = derive(x0, 0);
		// laske f(0)+(x/2)*f'(0)
		auto dx1 = add(x0, mul(dt/S(2), dx0));
		// Nyt voimme soveltaa keskipistemenetelmää
		// Kysy paljonko on f'(x/2), kun f(x/2) tiedetään
		dx0 = derive(dx1, dt/S(2));
		// Laske paljonko on f(0)+x*f'(x/2)
		return add(x0, mul(dt, dx0));
	}

	template<typename S, typename V, typename F>
	auto rungeKutta(const V& x0, S dt, F derive) {
		auto k1 = mul(dt, derive(x0, 0));
		auto t2 = add(x0, mul(S(0.5), k1));
		auto k2 = mul(dt, derive(t2, S(0.5) * dt));
		auto t3 = add(x0, mul(S(0.5), k2));
		auto k3 = mul(dt, derive(t3, S(0.5) * dt));
		auto t4 = add(x0, k3);
		auto k4 = mul(dt, derive(t4, dt));
		return add(add(add(add(x0,
				mul(S(1.0/6.0), k1)),
				mul(S(1.0/3.0), k2)),
				mul(S(1.0/3.0), k3)),
				mul(S(1.0/6.0), k4));
	}
}

template<typename S>
struct Box
{
	glm::vec2 position;
	glm::vec2 V = glm::vec2(0, 0);
	glm::vec2 size;
	float mass;
	float angle;
	float restitution = 1.0f;
};

template<typename Box, typename S>
Box simulateB(Box box, S dt)
{
	auto dVel = [&box](const decltype(box.position)& x0, S dt)
	{
		return glm::vec2(0.0f, -9.81f);
	};
	box.V = integrate::euler<S>(box.V, dt, dVel);

	auto dPos = [&box](const decltype(box.position)& x0, S dt)
	{
		return box.V;
	};
	box.position = integrate::euler<S>(box.position, dt, dPos);

	//Collisions with walls
	
	// Collision with left border
	if (box.position[0] - box.size[0] < -10.0f)
	{
		box.position[0] = -10.0f + box.size[0];
		box.V = 0.9f * glm::reflect(box.V, glm::vec2(1.0f, 0.0f));
	}
	// Collision with right border
	if (box.position[0] + box.size[0] > 10.0f)
	{
		box.position[0] = 10.0f - box.size[0];
		box.V = 0.9f * glm::reflect(box.V, glm::vec2(1.0f, 0.0f));
	}
	// Collision with bottom
	if (box.position[1] - box.size[1] < -10.0f)
	{
		box.position[1] = -10.0f + box.size[1];
		box.V = 1.0f * glm::reflect(box.V, glm::vec2(0.0f, 1.0f));
	}
	// Collision with top
	if (box.position[1] + box.size[1] > 10.0f)
	{
		box.position[1] = 10.0f - box.size[1];
		box.V = 0.2f * glm::reflect(box.V, glm::vec2(0.0f, 1.0f));
	}
	
	return box;
}

// Stuff for SAT
std::vector<glm::vec2> calculatedBounds(glm::vec2 position, glm::vec2 size, float angle)
{
	std::vector<glm::vec2> points;
	points.push_back(position);
	//	-sin +cos, +cos +sin TOP RIGHT
	//	+sin +cos, -cos +sin BOTTOM RIGHT
	//	+sin -cos, -cos -sin BOTTOM LEFT
	//	-sin -cos, +cos -sin TOP LEFT

	points.push_back(glm::vec2({ position.x - size[0] * sin(angle) + size[0] * cos(angle), position[1] + size[1] * cos(angle) + size[1] * sin(angle) })); // top right
	points.push_back(glm::vec2({ position.x + size[0] * sin(angle) + size[0] * cos(angle), position[1] - size[1] * cos(angle) + size[1] * sin(angle) })); // bottom right
	points.push_back(glm::vec2({ position.x + size[0] * sin(angle) - size[0] * cos(angle), position[1] - size[1] * cos(angle) - size[1] * sin(angle) })); // bottom left
	points.push_back(glm::vec2({ position.x - size[0] * sin(angle) - size[0] * cos(angle), position[1] + size[1] * cos(angle) - size[1] * sin(angle) })); // top left

	return points;
}

// Returns left normal
glm::vec2 normalLeft(const glm::vec2& vector)
{
	glm::vec2 normal = {
		-vector.y,
		vector.x
	};

	return normal;
}

glm::vec2 getMinMax(const std::vector<glm::vec2>& points, const glm::vec2 normal)
{
	float minproj = glm::dot(points[1], normal);
	float mindot = 1;
	float maxproj = glm::dot(points[1], normal);
	float maxdot = 1;
	for (size_t i = 2; i < points.size(); i++)
	{
		float currentA = glm::dot(points[i], normal);

		if (minproj > currentA)
		{
			minproj = currentA;
			mindot = i;
		}
		if (maxproj < currentA)
		{
			maxproj = currentA;
			maxdot = i;
		}
	}
	return glm::vec2({minproj, maxproj});
}

/// SAT
/// https://gamedevelopment.tutsplus.com/collision-detection-using-the-separating-axis-theorem--gamedev-169t

// SAT, Returns overlap
template<typename S>
bool isOverlappingNormal(const std::vector<S>& points1, const std::vector<S>& points2, const S normal)
{
	
	float minproj1 = glm::dot(points1[1], normal);
	float mindot1 = 1;
	float maxproj1 = glm::dot(points1[1], normal);
	float maxdot1 = 1;

	float minproj2 = glm::dot(points2[1], normal);
	float mindot2 = 1;
	float maxproj2 = glm::dot(points2[1], normal);
	float maxdot2 = 1;
	for (size_t j = 1; j < points1.size(); j++)
	{
		float currentA = glm::dot(points1[j], normal);

		if (minproj1 > currentA)
		{
			minproj1 = currentA;
			mindot1 = j;
		}
		if (maxproj1 < currentA)
		{
			maxproj1 = currentA;
			maxdot1 = j;
		}
	}
	for (size_t j = 1; j < points2.size(); j++)
	{
		float currentB = glm::dot(points2[j], normal);

		if (minproj2 > currentB)
		{
			minproj2 = currentB;
			mindot2 = j;
		}
		if (maxproj2 < currentB)
		{
			maxproj2 = currentB;
			maxdot2 = j;
		}
	}
	if (maxproj2 > minproj1 && maxproj1 > minproj2)
	{
		return true;
	}
	
	return false;
}

// Check intersection using normals from single object
template<typename S>
bool isIntersecting(const std::vector<S>& points1, const std::vector<S>& points2, const std::vector<S>& normals)
{
	// for penetration lenght, normalize normal
	// glm::vec2 axis = glm::normalize(glm::vec2(1, -1));
	for (size_t i = 0; i < normals.size(); i++)
	{
		if (!isOverlappingNormal(points1, points2, normals[i]))
		{
			return false;
		}
	}
	return true;
}

// Check intersection using normals from both objects
template<typename S>
bool isIntersecting(const std::vector<S>& points1, const std::vector<S>& points2, const std::vector<S>& normals1, const std::vector<S>& normals2)
{
	if (isIntersecting(points1, points2, normals1))
	{
		if (isIntersecting(points1, points2, normals2))
		{
			return true;
		}
	}

	return false;
}

//Points1 is a polygon, Points2 is a circle
template<typename S>
bool isIntersectingCircle(const std::vector<S>& points1, const std::vector<S>& points2, float radius)
{
	glm::vec2 circle = { points2[0].x - points1[0].x, points2[0].y - points1[0].y };
	float maximum = 0;
	for (size_t i = 1; i < points1.size(); i++)
	{
		glm::vec2 v = { points1[i].x - points1[0].x, points1[i].y - points1[0].y };
		float current = glm::dot(v, glm::normalize(circle));
		if (maximum < current)
		{
			maximum = current;
			
		}	
	}

	float mag = sqrtf(circle.x * circle.x + circle.y + circle.y);
	if (!(mag -maximum - radius > 0 && mag > 0))
	{
		return true;
	}
	return false;
}

// Returns normals from given points
std::vector<glm::vec2> getNormals(const std::vector<glm::vec2>& points)
{
	std::vector<glm::vec2> normals;
	for (size_t i = 1; i < points.size() - 1; i++)
	{
		glm::vec2 current = normalLeft(glm::vec2({ points[i + 1].x - points[i].x, points[i + 1].y - points[i].y }));
		normals.push_back(current);
	}
	normals.push_back(normalLeft(glm::vec2({ points[1].x - points[points.size() - 1].x, points[1].y - points[points.size() - 1].y })));

	return normals;
}

template<typename S>
std::vector<Box<S>> simulateBoxes(std::vector<Box<S>> boxes, S dt)
{
	for (auto& box : boxes)
	{
		box = simulateB(box, dt);
	}
	
	/// Reading examples for SAT
	/// https://gamedevelopment.tutsplus.com/collision-detection-using-the-separating-axis-theorem--gamedev-169t
	/// http://www.metanetsoftware.com/technique/tutorialA.html

	std::vector<std::vector<glm::vec2>> vectors;
	std::vector<std::vector<glm::vec2>> norms;

	for (int i = 0; i < boxes.size(); i++)
	{
		vectors.push_back(calculatedBounds(boxes[i].position, boxes[i].size, boxes[i].angle));
		norms.push_back(getNormals(vectors[i]));
	}

	for (int i = 0; i < boxes.size() - 1; i++)
	{
		for (int j = 1; j < boxes.size(); j++)
		{
			if (j != i && isIntersecting(vectors[i], vectors[j], norms[i], norms[j]))
			{	
				auto& b1 = boxes[i];
				auto& b2 = boxes[j];
				auto delta = b1.position - b2.position;
				auto normal = glm::normalize(delta);
				auto velocity = b1.V - b2.V;
				float invMass = (1.0f / b1.mass) + (1.0f / b2.mass);
				float e = b1.restitution;
				float j = -(1.0f + e) * glm::dot(velocity, normal) / (glm::dot(delta, delta) * invMass);
				b1.V += j / b1.mass * normal;
				b2.V -= j / b2.mass * normal;
			}
		}		
	}

	return boxes;
}

int main() {
	srand(time(NULL));
	using namespace mikroplot;

	Window window(900, 900, "Integrate point");
	Timer timer;

	std::vector<vec2> lines;
	std::vector<std::vector<vec2>> ver;

	std::vector<Box<float>> boxes = {
		{{3.0f, 3.0f}, {1.0f, 0.0f}, {0.5f, 0.5f}, 2.0f, 0.0f},
		{{6.0f, 1.0f}, {0.0f, 0.0f},{0.5f, 0.5f}, 1.0f, 0.0f},
		{{5.0f, 6.0f}, {0.0f, 0.0f},{0.5f, 0.5f}, 1.0f, 0.0f}
	};

	float totalTime = 0;
	while (!window.shouldClose()) 
	{	
		float dt = timer.getDeltaTime();
		totalTime += dt;
		int N = (int)totalTime;

		if (window.getKeyPressed(KEY_W))
		{
			boxes.push_back({ {5.0f, 5.0f}, {1.0f, 0.0f}, {0.5f, 0.5f}, 1.0f, 0.0f });
		}
		if (window.getKeyPressed(KEY_S))
		{
			if (boxes.size() > 1)
			{
				boxes.pop_back();
			}
		}
		if (window.getKeyPressed(KEY_A))
		{
			for (auto& box : boxes)
			{
				box.angle += 1.0f + rand();
			}
		}
		if (window.getKeyPressed(KEY_D))
		{
			for (auto& box : boxes)
			{
				box.angle -= 1.0f + rand();
			}
		}
		if (window.getKeyState(KEY_G))
		{
			for (auto& box : boxes)
			{
				box.V -= glm::vec2(0.0f, 1.0f);
				printf("Vertical velocity %f\n", box.V.y);
			}
		}
		if (window.getKeyState(KEY_T))
		{
			for (auto& box : boxes )
			{
				box.V += glm::vec2(0.0f, 1.0f);
				printf("Vertical velocity %f\n", box.V.y);
			}
		}
		if (window.getKeyState(KEY_F))
		{
			for (auto& box : boxes)
			{
				box.V -= glm::vec2(1.0f, 0.0f);
				printf("Horizontal velocity %f\n", box.V.x);
			}
		}
		if (window.getKeyState(KEY_H))
		{
			for (auto& box : boxes)
			{
				box.V += glm::vec2(1.0f, 0.0f);
				printf("Horizontal velocity %f\n", box.V.x);
			}
		}
		if (window.getKeyState(KEY_Y))
		{
			for (auto& box : boxes)
			{
				if (box.restitution < 1.0f)
				{
					box.restitution += 0.01f;
					printf("Restitution %f\n", box.restitution);
				}	
			}
		}
		if (window.getKeyState(KEY_U))
		{
			for (auto& box : boxes)
			{
				if (box.restitution > 0.0f)
				{
					box.restitution -= 0.01f;
					printf("Restitution %f\n", box.restitution);
				}	
			}
		}

		boxes = simulateBoxes(boxes, dt);

		// Render
		window.setScreen(-10, 10, -11, 10);
		window.drawAxis();
		ver.clear();
		

		for (int i = 0; i < boxes.size(); i++)
		{
			lines.clear();
			lines.push_back(vec2({ boxes[i].position[0] - boxes[i].size[0] * sin(boxes[i].angle) + boxes[i].size[0] * cos(boxes[i].angle), boxes[i].position[1] + boxes[i].size[1] * cos(boxes[i].angle) + boxes[i].size[1] * sin(boxes[i].angle) })); // top right
			lines.push_back(vec2({ boxes[i].position[0] + boxes[i].size[0] * sin(boxes[i].angle) + boxes[i].size[0] * cos(boxes[i].angle), boxes[i].position[1] - boxes[i].size[1] * cos(boxes[i].angle) + boxes[i].size[1] * sin(boxes[i].angle) })); // bottom right
			lines.push_back(vec2({ boxes[i].position[0] + boxes[i].size[0] * sin(boxes[i].angle) - boxes[i].size[0] * cos(boxes[i].angle), boxes[i].position[1] - boxes[i].size[1] * cos(boxes[i].angle) - boxes[i].size[1] * sin(boxes[i].angle) })); // bottom left
			lines.push_back(vec2({ boxes[i].position[0] - boxes[i].size[0] * sin(boxes[i].angle) - boxes[i].size[0] * cos(boxes[i].angle), boxes[i].position[1] + boxes[i].size[1] * cos(boxes[i].angle) - boxes[i].size[1] * sin(boxes[i].angle) })); // top left

			lines.push_back(vec2({ boxes[i].position[0] - boxes[i].size[0] * sin(boxes[i].angle) + boxes[i].size[0] * cos(boxes[i].angle), boxes[i].position[1] + boxes[i].size[1] * cos(boxes[i].angle) + boxes[i].size[1] * sin(boxes[i].angle) }));		
			ver.push_back(lines);
		}
		
		for (int i = 0; i < ver.size(); i++)
		{
			window.drawLines(ver[i], 11, 1);
		}		
		window.update();		
	}

	return 0;
}
















