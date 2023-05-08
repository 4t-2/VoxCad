#include "../lib/AGL/agl.hpp"

int main()
{
	agl::RenderWindow window;
	window.setup({1920, 1080}, "VoxCad");
	window.setClearColor(agl::Color::Black);
	window.setFPS(60);
	window.setSwapInterval(1);

	agl::Event event;
	event.setWindow(window);

	agl::ShaderBuilder sbFrag;
	{
		using namespace agl;

		val UVcoord	  = sbFrag.addIn(agl::vec2, "UVcoord");
		val fragColor = sbFrag.addIn(agl::vec4, "fragColor");
		val pos		  = sbFrag.addIn(agl::vec3, "pos");

		val color = sbFrag.addOut(agl::vec4, "color");

		val myTextureSampler = sbFrag.addUniform(agl::sampler2D, "myTextureSampler");

		sbFrag.setMain({
			color = agl::val("vec4(pos, 1.)") * agl::val("texture(myTextureSampler, UVcoord)"), //
		});
	}

	std::string fragSrc = sbFrag.getSrc();

	agl::ShaderBuilder sbVert;
	{
		using namespace agl;

		val position = sbVert.addLayout(0, agl::vec3, "position");
		val vertexUV = sbVert.addLayout(1, agl::vec2, "vertexUV");

		val transform		 = sbVert.addUniform(agl::mat4, "transform");
		val mvp				 = sbVert.addUniform(agl::mat4, "mvp");
		val shapeColor		 = sbVert.addUniform(agl::vec3, "shapeColor");
		val textureTransform = sbVert.addUniform(agl::mat4, "textureTransform");

		val UVcoord	  = sbVert.addOut(agl::vec2, "UVcoord");
		val fragColor = sbVert.addOut(agl::vec4, "fragColor");
		val pos		  = sbVert.addOut(agl::vec3, "pos");

		sbVert.setMain({
			UVcoord				  = val("vec2((textureTransform") * val("vec4(vertexUV, 1, 1)).xy)"), //
			fragColor			  = val("vec4(shapeColor, 1)"),										  //
			pos					  = position,
			val(val::gl_Position) = mvp * transform * val("vec4(position, 1)"), //
		});
	}

	std::string vertSrc = sbVert.getSrc();

	agl::Shader shader;
	shader.compileSrc(vertSrc, fragSrc);

	window.getShaderUniforms(shader);
	shader.use();

	agl::Camera camera;
	camera.setView({5, 5, 5}, {0, 0, 0}, {0, 1, 0});
	camera.setPerspectiveProjection(45, 1920. / 1080., .1, 100);

	float angle = 0;
	float scale = 5;

	window.updateMvp(camera);

	agl::Texture blank;
	blank.setBlank();

	agl::Cuboid cuboid;
	cuboid.setColor(agl::Color::Cyan);
	cuboid.setTexture(&blank);
	cuboid.setPosition({0, 0, 0});
	cuboid.setSize({1, 1, 1});

	while (!event.windowClose())
	{
		event.poll();

		window.clear();

		window.drawShape(cuboid);

		window.display();

		if (event.isKeyPressed(XK_Left))
		{
			angle += PI / 60;
		}

		if (event.isKeyPressed(XK_Right))
		{
			angle -= PI / 60;
		}

		if(event.isKeyPressed(XK_Up))
		{
			scale /= 1.05;
		}
		
		if(event.isKeyPressed(XK_Down))
		{
			scale *= 1.05;
		}

		agl::Vec<float, 2> normal = agl::pointOnCircle(angle);

		camera.setView(agl::Vec<float, 3>{normal.x, 1, normal.y} * scale, {0, 0, 0}, {0, 1, 0});
		window.updateMvp(camera);
	}

	blank.deleteTexture();
	shader.deleteProgram();
	window.close();

	return 0;
}
