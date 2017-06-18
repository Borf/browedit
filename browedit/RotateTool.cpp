#include "RotateTool.h"
#include "BrowEdit.h"
#include <blib/Shapes.h>
#include <blib/Shader.h>
#include <blib/Renderer.h>

#include <blib/linq.h>
#include <glm/gtc/matrix_transform.hpp>



RotatorTool::RotatorTool()
{
	//// build up arrow
	{
		std::vector<glm::vec3> verts;
		verts.push_back(glm::vec3(0.836100, 0.835598, 10.379000)); verts.push_back(glm::vec3(0.769100, 3.931098, 8.954101)); verts.push_back(glm::vec3(-0.769100, 3.931098, 8.954101));
		verts.push_back(glm::vec3(-0.769100, 3.931098, 8.954101)); verts.push_back(glm::vec3(-0.836100, 0.835598, 10.379000)); verts.push_back(glm::vec3(0.836100, 0.835598, 10.379000));
		verts.push_back(glm::vec3(0.836100, 0.835598, 10.379000)); verts.push_back(glm::vec3(1.038300, 0.054898, 9.960600)); verts.push_back(glm::vec3(1.121500, 2.505899, 7.821900));
		verts.push_back(glm::vec3(1.121500, 2.505899, 7.821900)); verts.push_back(glm::vec3(0.769100, 3.931098, 8.954101)); verts.push_back(glm::vec3(0.836100, 0.835598, 10.379000));
		verts.push_back(glm::vec3(-1.038300, 0.054898, 9.960600)); verts.push_back(glm::vec3(-1.121500, 2.505899, 7.821900)); verts.push_back(glm::vec3(1.121500, 2.505899, 7.821900));
		verts.push_back(glm::vec3(1.121500, 2.505899, 7.821900)); verts.push_back(glm::vec3(1.038300, 0.054898, 9.960600)); verts.push_back(glm::vec3(-1.038300, 0.054898, 9.960600));
		verts.push_back(glm::vec3(-0.836100, 0.835598, 10.379000)); verts.push_back(glm::vec3(-0.769100, 3.931098, 8.954101)); verts.push_back(glm::vec3(-1.121500, 2.505899, 7.821900));
		verts.push_back(glm::vec3(-1.121500, 2.505899, 7.821900)); verts.push_back(glm::vec3(-1.038300, 0.054898, 9.960600)); verts.push_back(glm::vec3(-0.836100, 0.835598, 10.379000));
		verts.push_back(glm::vec3(0.769100, 3.931098, 8.954101)); verts.push_back(glm::vec3(0.769100, 6.375099, 6.836301)); verts.push_back(glm::vec3(-0.769100, 6.375099, 6.836301));
		verts.push_back(glm::vec3(-0.769100, 6.375099, 6.836301)); verts.push_back(glm::vec3(-0.769100, 3.931098, 8.954101)); verts.push_back(glm::vec3(0.769100, 3.931098, 8.954101));
		verts.push_back(glm::vec3(0.769100, 3.931098, 8.954101)); verts.push_back(glm::vec3(1.121500, 2.505899, 7.821900)); verts.push_back(glm::vec3(1.121500, 4.542399, 6.057301));
		verts.push_back(glm::vec3(1.121500, 4.542399, 6.057301)); verts.push_back(glm::vec3(0.769100, 6.375099, 6.836301)); verts.push_back(glm::vec3(0.769100, 3.931098, 8.954101));
		verts.push_back(glm::vec3(-1.121500, 2.505899, 7.821900)); verts.push_back(glm::vec3(-1.121500, 4.542399, 6.057301)); verts.push_back(glm::vec3(1.121500, 4.542399, 6.057301));
		verts.push_back(glm::vec3(1.121500, 4.542399, 6.057301)); verts.push_back(glm::vec3(1.121500, 2.505899, 7.821900)); verts.push_back(glm::vec3(-1.121500, 2.505899, 7.821900));
		verts.push_back(glm::vec3(-0.769100, 3.931098, 8.954101)); verts.push_back(glm::vec3(-0.769100, 6.375099, 6.836301)); verts.push_back(glm::vec3(-1.121500, 4.542399, 6.057301));
		verts.push_back(glm::vec3(-1.121500, 4.542399, 6.057301)); verts.push_back(glm::vec3(-1.121500, 2.505899, 7.821900)); verts.push_back(glm::vec3(-0.769100, 3.931098, 8.954101));
		verts.push_back(glm::vec3(0.769100, 6.375099, 6.836301)); verts.push_back(glm::vec3(0.769100, 8.123499, 4.115801)); verts.push_back(glm::vec3(-0.769100, 8.123499, 4.115801));
		verts.push_back(glm::vec3(-0.769100, 8.123499, 4.115801)); verts.push_back(glm::vec3(-0.769100, 6.375099, 6.836301)); verts.push_back(glm::vec3(0.769100, 6.375099, 6.836301));
		verts.push_back(glm::vec3(0.769100, 6.375099, 6.836301)); verts.push_back(glm::vec3(1.121500, 4.542399, 6.057301)); verts.push_back(glm::vec3(1.121500, 5.999199, 3.790501));
		verts.push_back(glm::vec3(1.121500, 5.999199, 3.790501)); verts.push_back(glm::vec3(0.769100, 8.123499, 4.115801)); verts.push_back(glm::vec3(0.769100, 6.375099, 6.836301));
		verts.push_back(glm::vec3(-1.121500, 4.542399, 6.057301)); verts.push_back(glm::vec3(-1.121500, 5.999199, 3.790501)); verts.push_back(glm::vec3(1.121500, 5.999199, 3.790501));
		verts.push_back(glm::vec3(1.121500, 5.999199, 3.790501)); verts.push_back(glm::vec3(1.121500, 4.542399, 6.057301)); verts.push_back(glm::vec3(-1.121500, 4.542399, 6.057301));
		verts.push_back(glm::vec3(-0.769100, 6.375099, 6.836301)); verts.push_back(glm::vec3(-0.769100, 8.123499, 4.115801)); verts.push_back(glm::vec3(-1.121500, 5.999199, 3.790501));
		verts.push_back(glm::vec3(-1.121500, 5.999199, 3.790501)); verts.push_back(glm::vec3(-1.121500, 4.542399, 6.057301)); verts.push_back(glm::vec3(-0.769100, 6.375099, 6.836301));
		verts.push_back(glm::vec3(0.769100, 8.123499, 4.115801)); verts.push_back(glm::vec3(0.769100, 9.034600, 1.012901)); verts.push_back(glm::vec3(-0.769100, 9.034600, 1.012901));
		verts.push_back(glm::vec3(-0.769100, 9.034600, 1.012901)); verts.push_back(glm::vec3(-0.769100, 8.123499, 4.115801)); verts.push_back(glm::vec3(0.769100, 8.123499, 4.115801));
		verts.push_back(glm::vec3(0.769100, 8.123499, 4.115801)); verts.push_back(glm::vec3(1.121500, 5.999199, 3.790501)); verts.push_back(glm::vec3(1.121500, 6.758300, 1.205101));
		verts.push_back(glm::vec3(1.121500, 6.758300, 1.205101)); verts.push_back(glm::vec3(0.769100, 9.034600, 1.012901)); verts.push_back(glm::vec3(0.769100, 8.123499, 4.115801));
		verts.push_back(glm::vec3(-1.121500, 5.999199, 3.790501)); verts.push_back(glm::vec3(-1.121500, 6.758300, 1.205101)); verts.push_back(glm::vec3(1.121500, 6.758300, 1.205101));
		verts.push_back(glm::vec3(1.121500, 6.758300, 1.205101)); verts.push_back(glm::vec3(1.121500, 5.999199, 3.790501)); verts.push_back(glm::vec3(-1.121500, 5.999199, 3.790501));
		verts.push_back(glm::vec3(-0.769100, 8.123499, 4.115801)); verts.push_back(glm::vec3(-0.769100, 9.034600, 1.012901)); verts.push_back(glm::vec3(-1.121500, 6.758300, 1.205101));
		verts.push_back(glm::vec3(-1.121500, 6.758300, 1.205101)); verts.push_back(glm::vec3(-1.121500, 5.999199, 3.790501)); verts.push_back(glm::vec3(-0.769100, 8.123499, 4.115801));
		verts.push_back(glm::vec3(0.769100, 9.034600, 1.012901)); verts.push_back(glm::vec3(0.769100, 9.034600, -2.220999)); verts.push_back(glm::vec3(-0.769100, 9.034600, -2.220999));
		verts.push_back(glm::vec3(-0.769100, 9.034600, -2.220999)); verts.push_back(glm::vec3(-0.769100, 9.034600, 1.012901)); verts.push_back(glm::vec3(0.769100, 9.034600, 1.012901));
		verts.push_back(glm::vec3(0.769100, 9.034600, 1.012901)); verts.push_back(glm::vec3(1.121500, 6.758300, 1.205101)); verts.push_back(glm::vec3(1.121500, 6.758300, -1.489499));
		verts.push_back(glm::vec3(1.121500, 6.758300, -1.489499)); verts.push_back(glm::vec3(0.769100, 9.034600, -2.220999)); verts.push_back(glm::vec3(0.769100, 9.034600, 1.012901));
		verts.push_back(glm::vec3(-1.121500, 6.758300, 1.205101)); verts.push_back(glm::vec3(-1.121500, 6.758300, -1.489499)); verts.push_back(glm::vec3(1.121500, 6.758300, -1.489499));
		verts.push_back(glm::vec3(1.121500, 6.758300, -1.489499)); verts.push_back(glm::vec3(1.121500, 6.758300, 1.205101)); verts.push_back(glm::vec3(-1.121500, 6.758300, 1.205101));
		verts.push_back(glm::vec3(-0.769100, 9.034600, 1.012901)); verts.push_back(glm::vec3(-0.769100, 9.034600, -2.220999)); verts.push_back(glm::vec3(-1.121500, 6.758300, -1.489499));
		verts.push_back(glm::vec3(-1.121500, 6.758300, -1.489499)); verts.push_back(glm::vec3(-1.121500, 6.758300, 1.205101)); verts.push_back(glm::vec3(-0.769100, 9.034600, 1.012901));
		verts.push_back(glm::vec3(0.769100, 9.034600, -2.220999)); verts.push_back(glm::vec3(0.769100, 8.123501, -5.323899)); verts.push_back(glm::vec3(-0.769100, 8.123501, -5.323899));
		verts.push_back(glm::vec3(-0.769100, 8.123501, -5.323899)); verts.push_back(glm::vec3(-0.769100, 9.034600, -2.220999)); verts.push_back(glm::vec3(0.769100, 9.034600, -2.220999));
		verts.push_back(glm::vec3(0.769100, 9.034600, -2.220999)); verts.push_back(glm::vec3(1.121500, 6.758300, -1.489499)); verts.push_back(glm::vec3(1.121500, 5.999200, -4.074899));
		verts.push_back(glm::vec3(1.121500, 5.999200, -4.074899)); verts.push_back(glm::vec3(0.769100, 8.123501, -5.323899)); verts.push_back(glm::vec3(0.769100, 9.034600, -2.220999));
		verts.push_back(glm::vec3(-1.121500, 6.758300, -1.489499)); verts.push_back(glm::vec3(-1.121500, 5.999200, -4.074899)); verts.push_back(glm::vec3(1.121500, 5.999200, -4.074899));
		verts.push_back(glm::vec3(1.121500, 5.999200, -4.074899)); verts.push_back(glm::vec3(1.121500, 6.758300, -1.489499)); verts.push_back(glm::vec3(-1.121500, 6.758300, -1.489499));
		verts.push_back(glm::vec3(-0.769100, 9.034600, -2.220999)); verts.push_back(glm::vec3(-0.769100, 8.123501, -5.323899)); verts.push_back(glm::vec3(-1.121500, 5.999200, -4.074899));
		verts.push_back(glm::vec3(-1.121500, 5.999200, -4.074899)); verts.push_back(glm::vec3(-1.121500, 6.758300, -1.489499)); verts.push_back(glm::vec3(-0.769100, 9.034600, -2.220999));
		verts.push_back(glm::vec3(0.769100, 8.123501, -5.323899)); verts.push_back(glm::vec3(0.769100, 6.375102, -8.044399)); verts.push_back(glm::vec3(-0.769100, 6.375102, -8.044399));
		verts.push_back(glm::vec3(-0.769100, 6.375102, -8.044399)); verts.push_back(glm::vec3(-0.769100, 8.123501, -5.323899)); verts.push_back(glm::vec3(0.769100, 8.123501, -5.323899));
		verts.push_back(glm::vec3(0.769100, 8.123501, -5.323899)); verts.push_back(glm::vec3(1.121500, 5.999200, -4.074899)); verts.push_back(glm::vec3(1.121500, 4.542401, -6.341699));
		verts.push_back(glm::vec3(1.121500, 4.542401, -6.341699)); verts.push_back(glm::vec3(0.769100, 6.375102, -8.044399)); verts.push_back(glm::vec3(0.769100, 8.123501, -5.323899));
		verts.push_back(glm::vec3(-1.121500, 5.999200, -4.074899)); verts.push_back(glm::vec3(-1.121500, 4.542401, -6.341699)); verts.push_back(glm::vec3(1.121500, 4.542401, -6.341699));
		verts.push_back(glm::vec3(1.121500, 4.542401, -6.341699)); verts.push_back(glm::vec3(1.121500, 5.999200, -4.074899)); verts.push_back(glm::vec3(-1.121500, 5.999200, -4.074899));
		verts.push_back(glm::vec3(-0.769100, 8.123501, -5.323899)); verts.push_back(glm::vec3(-0.769100, 6.375102, -8.044399)); verts.push_back(glm::vec3(-1.121500, 4.542401, -6.341699));
		verts.push_back(glm::vec3(-1.121500, 4.542401, -6.341699)); verts.push_back(glm::vec3(-1.121500, 5.999200, -4.074899)); verts.push_back(glm::vec3(-0.769100, 8.123501, -5.323899));
		verts.push_back(glm::vec3(0.769100, 6.375102, -8.044399)); verts.push_back(glm::vec3(0.769100, 3.931102, -10.162199)); verts.push_back(glm::vec3(-0.769100, 3.931102, -10.162199));
		verts.push_back(glm::vec3(-0.769100, 3.931102, -10.162199)); verts.push_back(glm::vec3(-0.769100, 6.375102, -8.044399)); verts.push_back(glm::vec3(0.769100, 6.375102, -8.044399));
		verts.push_back(glm::vec3(1.121500, 4.542401, -6.341699)); verts.push_back(glm::vec3(1.121500, 2.505901, -8.106300)); verts.push_back(glm::vec3(0.769100, 3.931102, -10.162199));
		verts.push_back(glm::vec3(0.769100, 3.931102, -10.162199)); verts.push_back(glm::vec3(0.769100, 6.375102, -8.044399)); verts.push_back(glm::vec3(1.121500, 4.542401, -6.341699));
		verts.push_back(glm::vec3(-1.121500, 4.542401, -6.341699)); verts.push_back(glm::vec3(-1.121500, 2.505901, -8.106300)); verts.push_back(glm::vec3(1.121500, 2.505901, -8.106300));
		verts.push_back(glm::vec3(1.121500, 2.505901, -8.106300)); verts.push_back(glm::vec3(1.121500, 4.542401, -6.341699)); verts.push_back(glm::vec3(-1.121500, 4.542401, -6.341699));
		verts.push_back(glm::vec3(-0.769100, 6.375102, -8.044399)); verts.push_back(glm::vec3(-0.769100, 3.931102, -10.162199)); verts.push_back(glm::vec3(-1.121500, 2.505901, -8.106300));
		verts.push_back(glm::vec3(-1.121500, 2.505901, -8.106300)); verts.push_back(glm::vec3(-1.121500, 4.542401, -6.341699)); verts.push_back(glm::vec3(-0.769100, 6.375102, -8.044399));
		verts.push_back(glm::vec3(0.769100, 3.931102, -10.162199)); verts.push_back(glm::vec3(0.769100, 0.989502, -11.505600)); verts.push_back(glm::vec3(-0.769100, 0.989502, -11.505600));
		verts.push_back(glm::vec3(-0.769100, 0.989502, -11.505600)); verts.push_back(glm::vec3(-0.769100, 3.931102, -10.162199)); verts.push_back(glm::vec3(0.769100, 3.931102, -10.162199));
		verts.push_back(glm::vec3(1.121500, 2.505901, -8.106300)); verts.push_back(glm::vec3(1.121500, 0.054902, -9.225700)); verts.push_back(glm::vec3(0.769100, 0.989502, -11.505600));
		verts.push_back(glm::vec3(0.769100, 0.989502, -11.505600)); verts.push_back(glm::vec3(0.769100, 3.931102, -10.162199)); verts.push_back(glm::vec3(1.121500, 2.505901, -8.106300));
		verts.push_back(glm::vec3(-1.121500, 2.505901, -8.106300)); verts.push_back(glm::vec3(-1.121500, 0.054902, -9.225700)); verts.push_back(glm::vec3(1.121500, 0.054902, -9.225700));
		verts.push_back(glm::vec3(1.121500, 0.054902, -9.225700)); verts.push_back(glm::vec3(1.121500, 2.505901, -8.106300)); verts.push_back(glm::vec3(-1.121500, 2.505901, -8.106300));
		verts.push_back(glm::vec3(-1.121500, 2.505901, -8.106300)); verts.push_back(glm::vec3(-0.769100, 3.931102, -10.162199)); verts.push_back(glm::vec3(-0.769100, 0.989502, -11.505600));
		verts.push_back(glm::vec3(-0.769100, 0.989502, -11.505600)); verts.push_back(glm::vec3(-1.121500, 0.054902, -9.225700)); verts.push_back(glm::vec3(-1.121500, 2.505901, -8.106300));
		verts.push_back(glm::vec3(0.769100, 0.989502, -11.505600)); verts.push_back(glm::vec3(0.769100, -2.211498, -11.965800)); verts.push_back(glm::vec3(-0.769100, -2.211498, -11.965800));
		verts.push_back(glm::vec3(-0.769100, -2.211498, -11.965800)); verts.push_back(glm::vec3(-0.769100, 0.989502, -11.505600)); verts.push_back(glm::vec3(0.769100, 0.989502, -11.505600));
		verts.push_back(glm::vec3(1.121500, 0.054902, -9.225700)); verts.push_back(glm::vec3(1.121500, -2.612298, -9.609100)); verts.push_back(glm::vec3(0.769100, -2.211498, -11.965800));
		verts.push_back(glm::vec3(0.769100, -2.211498, -11.965800)); verts.push_back(glm::vec3(0.769100, 0.989502, -11.505600)); verts.push_back(glm::vec3(1.121500, 0.054902, -9.225700));
		verts.push_back(glm::vec3(-1.121500, 0.054902, -9.225700)); verts.push_back(glm::vec3(-1.121500, -2.612298, -9.609100)); verts.push_back(glm::vec3(1.121500, -2.612298, -9.609100));
		verts.push_back(glm::vec3(1.121500, -2.612298, -9.609100)); verts.push_back(glm::vec3(1.121500, 0.054902, -9.225700)); verts.push_back(glm::vec3(-1.121500, 0.054902, -9.225700));
		verts.push_back(glm::vec3(-1.121500, 0.054902, -9.225700)); verts.push_back(glm::vec3(-0.769100, 0.989502, -11.505600)); verts.push_back(glm::vec3(-0.769100, -2.211498, -11.965800));
		verts.push_back(glm::vec3(-0.769100, -2.211498, -11.965800)); verts.push_back(glm::vec3(-1.121500, -2.612298, -9.609100)); verts.push_back(glm::vec3(-1.121500, 0.054902, -9.225700));
		verts.push_back(glm::vec3(0.769100, -2.211498, -11.965800)); verts.push_back(glm::vec3(0.769100, -5.412498, -11.505601)); verts.push_back(glm::vec3(-0.769100, -5.412498, -11.505601));
		verts.push_back(glm::vec3(-0.769100, -5.412498, -11.505601)); verts.push_back(glm::vec3(-0.769100, -2.211498, -11.965800)); verts.push_back(glm::vec3(0.769100, -2.211498, -11.965800));
		verts.push_back(glm::vec3(1.121500, -2.612298, -9.609100)); verts.push_back(glm::vec3(1.121500, -5.279398, -9.225701)); verts.push_back(glm::vec3(0.769100, -5.412498, -11.505601));
		verts.push_back(glm::vec3(0.769100, -5.412498, -11.505601)); verts.push_back(glm::vec3(0.769100, -2.211498, -11.965800)); verts.push_back(glm::vec3(1.121500, -2.612298, -9.609100));
		verts.push_back(glm::vec3(-1.121500, -2.612298, -9.609100)); verts.push_back(glm::vec3(-1.121500, -5.279398, -9.225701)); verts.push_back(glm::vec3(1.121500, -5.279398, -9.225701));
		verts.push_back(glm::vec3(1.121500, -5.279398, -9.225701)); verts.push_back(glm::vec3(1.121500, -2.612298, -9.609100)); verts.push_back(glm::vec3(-1.121500, -2.612298, -9.609100));
		verts.push_back(glm::vec3(-1.121500, -2.612298, -9.609100)); verts.push_back(glm::vec3(-0.769100, -2.211498, -11.965800)); verts.push_back(glm::vec3(-0.769100, -5.412498, -11.505601));
		verts.push_back(glm::vec3(-0.769100, -5.412498, -11.505601)); verts.push_back(glm::vec3(-1.121500, -5.279398, -9.225701)); verts.push_back(glm::vec3(-1.121500, -2.612298, -9.609100));
		verts.push_back(glm::vec3(0.769100, -5.412498, -11.505601)); verts.push_back(glm::vec3(0.769100, -8.354198, -10.162201)); verts.push_back(glm::vec3(-0.769100, -8.354198, -10.162201));
		verts.push_back(glm::vec3(-0.769100, -8.354198, -10.162201)); verts.push_back(glm::vec3(-0.769100, -5.412498, -11.505601)); verts.push_back(glm::vec3(0.769100, -5.412498, -11.505601));
		verts.push_back(glm::vec3(1.121500, -5.279398, -9.225701)); verts.push_back(glm::vec3(1.121500, -7.730499, -8.106301)); verts.push_back(glm::vec3(0.769100, -8.354198, -10.162201));
		verts.push_back(glm::vec3(0.769100, -8.354198, -10.162201)); verts.push_back(glm::vec3(0.769100, -5.412498, -11.505601)); verts.push_back(glm::vec3(1.121500, -5.279398, -9.225701));
		verts.push_back(glm::vec3(-1.121500, -5.279398, -9.225701)); verts.push_back(glm::vec3(-1.121500, -7.730499, -8.106301)); verts.push_back(glm::vec3(1.121500, -7.730499, -8.106301));
		verts.push_back(glm::vec3(1.121500, -7.730499, -8.106301)); verts.push_back(glm::vec3(1.121500, -5.279398, -9.225701)); verts.push_back(glm::vec3(-1.121500, -5.279398, -9.225701));
		verts.push_back(glm::vec3(-1.121500, -5.279398, -9.225701)); verts.push_back(glm::vec3(-0.769100, -5.412498, -11.505601)); verts.push_back(glm::vec3(-0.769100, -8.354198, -10.162201));
		verts.push_back(glm::vec3(-0.769100, -8.354198, -10.162201)); verts.push_back(glm::vec3(-1.121500, -7.730499, -8.106301)); verts.push_back(glm::vec3(-1.121500, -5.279398, -9.225701));
		verts.push_back(glm::vec3(0.769100, -8.354198, -10.162201)); verts.push_back(glm::vec3(0.769100, -10.798199, -8.044402)); verts.push_back(glm::vec3(-0.769100, -10.798199, -8.044402));
		verts.push_back(glm::vec3(-0.769100, -10.798199, -8.044402)); verts.push_back(glm::vec3(-0.769100, -8.354198, -10.162201)); verts.push_back(glm::vec3(0.769100, -8.354198, -10.162201));
		verts.push_back(glm::vec3(1.121500, -7.730499, -8.106301)); verts.push_back(glm::vec3(1.121500, -9.766899, -6.341702)); verts.push_back(glm::vec3(0.769100, -10.798199, -8.044402));
		verts.push_back(glm::vec3(0.769100, -10.798199, -8.044402)); verts.push_back(glm::vec3(0.769100, -8.354198, -10.162201)); verts.push_back(glm::vec3(1.121500, -7.730499, -8.106301));
		verts.push_back(glm::vec3(-1.121500, -7.730499, -8.106301)); verts.push_back(glm::vec3(-1.121500, -9.766899, -6.341702)); verts.push_back(glm::vec3(1.121500, -9.766899, -6.341702));
		verts.push_back(glm::vec3(1.121500, -9.766899, -6.341702)); verts.push_back(glm::vec3(1.121500, -7.730499, -8.106301)); verts.push_back(glm::vec3(-1.121500, -7.730499, -8.106301));
		verts.push_back(glm::vec3(-1.121500, -7.730499, -8.106301)); verts.push_back(glm::vec3(-0.769100, -8.354198, -10.162201)); verts.push_back(glm::vec3(-0.769100, -10.798199, -8.044402));
		verts.push_back(glm::vec3(-0.769100, -10.798199, -8.044402)); verts.push_back(glm::vec3(-1.121500, -9.766899, -6.341702)); verts.push_back(glm::vec3(-1.121500, -7.730499, -8.106301));
		verts.push_back(glm::vec3(0.769100, -10.798199, -8.044402)); verts.push_back(glm::vec3(0.769100, -12.546599, -5.323902)); verts.push_back(glm::vec3(-0.769100, -12.546599, -5.323902));
		verts.push_back(glm::vec3(-0.769100, -12.546599, -5.323902)); verts.push_back(glm::vec3(-0.769100, -10.798199, -8.044402)); verts.push_back(glm::vec3(0.769100, -10.798199, -8.044402));
		verts.push_back(glm::vec3(1.121500, -9.766899, -6.341702)); verts.push_back(glm::vec3(1.121500, -11.223699, -4.074902)); verts.push_back(glm::vec3(0.769100, -12.546599, -5.323902));
		verts.push_back(glm::vec3(0.769100, -12.546599, -5.323902)); verts.push_back(glm::vec3(0.769100, -10.798199, -8.044402)); verts.push_back(glm::vec3(1.121500, -9.766899, -6.341702));
		verts.push_back(glm::vec3(-1.121500, -9.766899, -6.341702)); verts.push_back(glm::vec3(-1.121500, -11.223699, -4.074902)); verts.push_back(glm::vec3(1.121500, -11.223699, -4.074902));
		verts.push_back(glm::vec3(1.121500, -11.223699, -4.074902)); verts.push_back(glm::vec3(1.121500, -9.766899, -6.341702)); verts.push_back(glm::vec3(-1.121500, -9.766899, -6.341702));
		verts.push_back(glm::vec3(-1.121500, -9.766899, -6.341702)); verts.push_back(glm::vec3(-0.769100, -10.798199, -8.044402)); verts.push_back(glm::vec3(-0.769100, -12.546599, -5.323902));
		verts.push_back(glm::vec3(-0.769100, -12.546599, -5.323902)); verts.push_back(glm::vec3(-1.121500, -11.223699, -4.074902)); verts.push_back(glm::vec3(-1.121500, -9.766899, -6.341702));
		verts.push_back(glm::vec3(0.769100, -12.546599, -5.323902)); verts.push_back(glm::vec3(0.659300, -13.255600, -1.459902)); verts.push_back(glm::vec3(-0.659300, -13.255600, -1.459902));
		verts.push_back(glm::vec3(-0.659300, -13.255600, -1.459902)); verts.push_back(glm::vec3(-0.769100, -12.546599, -5.323902)); verts.push_back(glm::vec3(0.769100, -12.546599, -5.323902));
		verts.push_back(glm::vec3(1.121500, -11.223699, -4.074902)); verts.push_back(glm::vec3(0.818800, -12.103900, -1.459902)); verts.push_back(glm::vec3(0.659300, -13.255600, -1.459902));
		verts.push_back(glm::vec3(0.659300, -13.255600, -1.459902)); verts.push_back(glm::vec3(0.769100, -12.546599, -5.323902)); verts.push_back(glm::vec3(1.121500, -11.223699, -4.074902));
		verts.push_back(glm::vec3(-1.121500, -11.223699, -4.074902)); verts.push_back(glm::vec3(-0.818800, -12.103900, -1.459902)); verts.push_back(glm::vec3(0.818800, -12.103900, -1.459902));
		verts.push_back(glm::vec3(0.818800, -12.103900, -1.459902)); verts.push_back(glm::vec3(1.121500, -11.223699, -4.074902)); verts.push_back(glm::vec3(-1.121500, -11.223699, -4.074902));
		verts.push_back(glm::vec3(-1.121500, -11.223699, -4.074902)); verts.push_back(glm::vec3(-0.769100, -12.546599, -5.323902)); verts.push_back(glm::vec3(-0.659300, -13.255600, -1.459902));
		verts.push_back(glm::vec3(-0.659300, -13.255600, -1.459902)); verts.push_back(glm::vec3(-0.818800, -12.103900, -1.459902)); verts.push_back(glm::vec3(-1.121500, -11.223699, -4.074902));
		verts.push_back(glm::vec3(0.659300, -13.255600, -1.459902)); verts.push_back(glm::vec3(2.223700, -15.050400, -1.459903)); verts.push_back(glm::vec3(-2.223700, -15.050400, -1.459903));
		verts.push_back(glm::vec3(-2.223700, -15.050400, -1.459903)); verts.push_back(glm::vec3(-0.659300, -13.255600, -1.459902)); verts.push_back(glm::vec3(0.659300, -13.255600, -1.459902));
		verts.push_back(glm::vec3(0.818800, -12.103900, -1.459902)); verts.push_back(glm::vec3(2.223700, -9.904400, -1.459902)); verts.push_back(glm::vec3(2.223700, -15.050400, -1.459903));
		verts.push_back(glm::vec3(2.223700, -15.050400, -1.459903)); verts.push_back(glm::vec3(0.659300, -13.255600, -1.459902)); verts.push_back(glm::vec3(0.818800, -12.103900, -1.459902));
		verts.push_back(glm::vec3(-0.818800, -12.103900, -1.459902)); verts.push_back(glm::vec3(-2.223700, -9.904400, -1.459902)); verts.push_back(glm::vec3(2.223700, -9.904400, -1.459902));
		verts.push_back(glm::vec3(2.223700, -9.904400, -1.459902)); verts.push_back(glm::vec3(0.818800, -12.103900, -1.459902)); verts.push_back(glm::vec3(-0.818800, -12.103900, -1.459902));
		verts.push_back(glm::vec3(-0.818800, -12.103900, -1.459902)); verts.push_back(glm::vec3(-0.659300, -13.255600, -1.459902)); verts.push_back(glm::vec3(-2.223700, -15.050400, -1.459903));
		verts.push_back(glm::vec3(-2.223700, -15.050400, -1.459903)); verts.push_back(glm::vec3(-2.223700, -9.904400, -1.459902)); verts.push_back(glm::vec3(-0.818800, -12.103900, -1.459902));
		verts.push_back(glm::vec3(-2.223700, -15.050400, -1.459903)); verts.push_back(glm::vec3(2.223700, -15.050400, -1.459903)); verts.push_back(glm::vec3(0.000000, -12.477401, 7.080598));
		verts.push_back(glm::vec3(2.223700, -15.050400, -1.459903)); verts.push_back(glm::vec3(2.223700, -9.904400, -1.459902)); verts.push_back(glm::vec3(0.000000, -12.477401, 7.080598));
		verts.push_back(glm::vec3(2.223700, -9.904400, -1.459902)); verts.push_back(glm::vec3(-2.223700, -9.904400, -1.459902)); verts.push_back(glm::vec3(0.000000, -12.477401, 7.080598));
		verts.push_back(glm::vec3(-2.223700, -9.904400, -1.459902)); verts.push_back(glm::vec3(-2.223700, -15.050400, -1.459903)); verts.push_back(glm::vec3(0.000000, -12.477401, 7.080598));
		verts.push_back(glm::vec3(0.836100, 0.835598, 10.379000)); verts.push_back(glm::vec3(-0.836100, 0.835598, 10.379000)); verts.push_back(glm::vec3(-1.038300, 0.054898, 9.960600));
		verts.push_back(glm::vec3(-1.038300, 0.054898, 9.960600)); verts.push_back(glm::vec3(1.038300, 0.054898, 9.960600)); verts.push_back(glm::vec3(0.836100, 0.835598, 10.379000));




		for (size_t i = 0; i < verts.size(); i += 3)
		{
			glm::vec3 normal = glm::normalize(glm::cross(verts[i + 1] - verts[i], verts[i + 2] - verts[i]));
			for (size_t ii = i; ii < i + 3; ii++)
				arrow.push_back(blib::VertexP3N3(verts[ii], normal));
		}
	}

}




void RotatorTool::draw(const blib::math::Ray& mouseRay, blib::RenderState& highlightRenderState, const glm::vec3 &center, const glm::mat4 &modelView, blib::Renderer* renderer)
{
	Axis collides = selectedAxis(mouseRay, center);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::texMult, glm::vec4(0, 0, 0, 0));
	highlightRenderState.activeTexture[0] = NULL;
	highlightRenderState.depthTest = true;


	glm::mat4 cameraMat = glm::translate(modelView, center);

	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::diffuse, 0.75f);


	glm::mat4 finalModelView = glm::rotate(cameraMat, glm::radians(90.0f), glm::vec3(0, 0, 1)); // Y axis
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModelView)));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::normalMatrix, normalMatrix);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(1, 0, 0, collides == Axis::Y ? 1 : 0.25f));
	renderer->drawTriangles(arrow, highlightRenderState);

	finalModelView = glm::rotate(cameraMat, glm::radians(0.0f), glm::vec3(1, 0, 0));			// x axis
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModelView)));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::normalMatrix, normalMatrix);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(0, 1, 0, collides == Axis::X ? 1 : 0.25f));
	renderer->drawTriangles(arrow, highlightRenderState);

	finalModelView = glm::rotate(cameraMat, glm::radians(90.0f), glm::vec3(0, 1, 0));			// z axis
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalModelView)));
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::normalMatrix, normalMatrix);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::color, glm::vec4(0, 0, 1, collides == Axis::Z ? 1 : 0.25f));
	renderer->drawTriangles(arrow, highlightRenderState);


	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::diffuse, 0.0f);
	highlightRenderState.activeShader->setUniform(BrowEdit::HighlightShaderUniforms::modelviewMatrix, finalModelView);

}



RotatorTool::Axis RotatorTool::selectedAxis(const blib::math::Ray& mouseRay, const glm::vec3 &center)
{
	float t;


	blib::math::Ray transformedRayX = mouseRay * glm::inverse(glm::rotate(glm::translate(glm::mat4(), center), glm::radians(0.0f), glm::vec3(1, 0, 0)));
	blib::math::Ray transformedRayY = mouseRay * glm::inverse(glm::rotate(glm::translate(glm::mat4(), center), glm::radians(90.0f), glm::vec3(0, 0, 1)));
	blib::math::Ray transformedRayZ = mouseRay * glm::inverse(glm::rotate(glm::translate(glm::mat4(), center), glm::radians(90.0f), glm::vec3(0, 1, 0)));

	std::vector<glm::vec3> polygon(3);
	for (size_t i = 0; i < arrow.size(); i += 3)
	{
		for (size_t ii = 0; ii < 3; ii++)
			polygon[ii] = arrow[i + ii].position;
		if (transformedRayX.LineIntersectPolygon(polygon, t))
			return Axis::X;
		if (transformedRayY.LineIntersectPolygon(polygon, t))
			return Axis::Y;
		if (transformedRayZ.LineIntersectPolygon(polygon, t))
			return Axis::Z;
	}

	return Axis::NONE;
}