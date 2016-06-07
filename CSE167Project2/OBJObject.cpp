#include <limits>
#include "OBJObject.h"
#include "Window.h"

using namespace std;

extern glm::vec4 light;
extern glm::vec4 pl_dir;
extern GLfloat pl_scale;
extern glm::vec4 spot_dir;
extern GLfloat spot_scale;
extern int lightType;
extern float range;
extern float spotExp;


struct material {
    glm::vec4 spec;
    GLfloat shine;
    glm::vec4 mat;
    glm::vec4 amb;
};
extern struct material * currentMat;

OBJObject::OBJObject(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals,std::vector<GLuint> indices)
{
    toWorld = glm::mat4(1.0f);
//    parse(filepath);
    this->vertices = vertices;
    this->normals = normals;
    this->indices = indices;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &(VBO2));
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    
    
    
    
}

OBJObject::OBJObject(const char *filepath)
{
	toWorld = glm::mat4(1.0f);
	parse(filepath);
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &(VBO2));
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO


    
    
}

OBJObject::~OBJObject()
{
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void OBJObject::parse1(const char *filepath)
{
    //TODO parse the OBJ file
    // Populate the face indices, vertices, and normals vectors with the OBJ Object data
    ifstream fp;     // file pointer
    float x, y, z;  // vertex coordinates
    float r, g, b;  // vertex color
    int v1,v2,v3;
    int vt1,vt2,vt3;
    int vn1,vn2,vn3;
    
    //float minX, minY, minZ , maxX, maxY, maxZ;
    float minX = std::numeric_limits<float>::max();
    float minY = minX, minZ = minX;
    float maxX = -minX, maxY = -minX, maxZ = -minX;
    
    float avgX, avgY, avgZ;
    
    string line;    // characters read from file
    
    fp.open(filepath);  // make the file name configurable so you can load other files
    while (fp.good())
    {
        
        std::getline(fp, line);
        
        if (line[0] == 'v')
        {
            if (line[1] == ' ')
            {
                sscanf(line.c_str(), "v %f %f %f\n", &x, &y, &z);
                vertices.push_back(glm::vec3(x,y,z));
                
                
            }
            
            else if(line[1] == 'n')
            {
                sscanf(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
                normals.push_back(glm::vec3(x,y,z));
                
            }
        }
        else if(line[0] == 'f')
        {
            sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &vt1,&vn1, &v2, &vt2,&vn2, &v3, &vt3,&vn3);
            indices.push_back(v1-1);
            indices.push_back(v2-1);
            indices.push_back(v3-1);
            
            
        }
        else
            continue;
        
    }
    fp.close();   // make sure you don't forget to close the file when done
    /*
    for(int i =0;i<vertices.size();i++)
    {
        if(vertices[i].x < minX)
            minX = vertices[i].x;
        if(vertices[i].x > maxX)
            maxX = vertices[i].x;
        if(vertices[i].y < minY)
            minY = vertices[i].y;
        if(vertices[i].y > maxY)
            maxY = vertices[i].y;
        if(vertices[i].z < minZ)
            minZ = vertices[i].z;
        if(vertices[i].z > maxZ)
            maxZ = vertices[i].z;
    }
    avgX = (maxX + minX)/2;
    avgY = (maxY + minY)/2;
    avgZ = (maxZ + minZ)/2;
    //std::cerr << avgX <<" "<< avgY <<" "<< avgZ << endl;
    for(int i =0;i<vertices.size();i++)
    {
        vertices[i].x = vertices[i].x-avgX;
        vertices[i].y = vertices[i].y-avgY;
        vertices[i].z = vertices[i].z-avgZ;
    }
    for(int i =0;i<vertices.size();i++)
    {
        vertices[i].x = vertices[i].x/(maxX-minX)*2;
        vertices[i].y = vertices[i].y/(maxY-minY)*2;
        vertices[i].z = vertices[i].z/(maxZ-minZ)*2;
    }*/
}

void OBJObject::parse(const char *filepath)
{
    //TODO parse the OBJ file
    // Populate the face indices, vertices, and normals vectors with the OBJ Object data
	ifstream fp;     // file pointer
	float x, y, z;  // vertex coordinates
	float r, g, b;  // vertex color
    int v1,v2,v3;
    
    //float minX, minY, minZ , maxX, maxY, maxZ;
    float minX = std::numeric_limits<float>::max();
    float minY = minX, minZ = minX;
    float maxX = -minX, maxY = -minX, maxZ = -minX;
    
    float avgX, avgY, avgZ;
    
	string line;    // characters read from file

	fp.open(filepath);  // make the file name configurable so you can load other files
	while (fp.good())
	{
        
		std::getline(fp, line);
       
		if (line[0] == 'v')
		{
			if (line[1] == ' ')
			{
				sscanf(line.c_str(), "v %f %f %f %f %f %f\n", &x, &y, &z, &r, &g, &b);
                vertices.push_back(glm::vec3(x,y,z));
                
                
			}
				
			else
			{
				sscanf(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
                normals.push_back(glm::vec3(x,y,z));
           
			}
		}
		else if(line[0] == 'f')
        {
            sscanf(line.c_str(), "f %d//%d %d//%d %d//%d\n", &v1, &v1, &v2, &v2, &v3, &v3);
            indices.push_back(v1-1);
            indices.push_back(v2-1);
            indices.push_back(v3-1);
            
            
        }
        else
            continue;
		
	}
	fp.close();   // make sure you don't forget to close the file when done
    
    for(int i =0;i<vertices.size();i++)
    {
        if(vertices[i].x < minX)
            minX = vertices[i].x;
        if(vertices[i].x > maxX)
            maxX = vertices[i].x;
        if(vertices[i].y < minY)
            minY = vertices[i].y;
        if(vertices[i].y > maxY)
            maxY = vertices[i].y;
        if(vertices[i].z < minZ)
            minZ = vertices[i].z;
        if(vertices[i].z > maxZ)
            maxZ = vertices[i].z;
    }
    avgX = (maxX + minX)/2;
    avgY = (maxY + minY)/2;
    avgZ = (maxZ + minZ)/2;
    //std::cerr << avgX <<" "<< avgY <<" "<< avgZ << endl;
    for(int i =0;i<vertices.size();i++)
    {
        vertices[i].x = vertices[i].x-avgX;
        vertices[i].y = vertices[i].y-avgY;
        vertices[i].z = vertices[i].z-avgZ;
    }
    for(int i =0;i<vertices.size();i++)
    {
        vertices[i].x = vertices[i].x/(maxX-minX)*2;
        vertices[i].y = vertices[i].y/(maxY-minY)*2;
        vertices[i].z = vertices[i].z/(maxZ-minZ)*2;
    }
}

void OBJObject::draw(GLuint shaderProgram, glm::mat4 M)
{
    // Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
    //glm::mat4 MVP = Window::P * Window::V * toWorld;
    glm::mat4 MVP = Window::P * Window::V * M;
    glm::mat4 MV = Window::V * M;
    glm::mat4 PV = Window::P * Window::V;
    glm::mat3 NM = glm::mat3(transpose(inverse(MV)));
    
    GLuint MID = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(MID, 1, GL_FALSE, &M[0][0]);
    
    // need ^ for diffuse specular and shininess
    // material.diffuse, material.specular, etc.
    
    GLuint MVID = glGetUniformLocation(shaderProgram, "m_viewModel");
    glUniformMatrix4fv(MVID, 1, GL_FALSE, &MV[0][0]);
    GLuint NMID = glGetUniformLocation(shaderProgram, "NM");
    glUniformMatrix3fv(NMID, 1, GL_FALSE, &NM[0][0]);
    GLuint AmID = glGetUniformLocation(shaderProgram, "material.ambient");
    glUniform3f(AmID, this->material.ambient.x,this->material.ambient.y,this->material.ambient.z);
    GLuint MatID = glGetUniformLocation(shaderProgram, "material.diffuse");
    glUniform3f(MatID, this->material.diffuse.x,this->material.diffuse.y,this->material.diffuse.z);
    GLuint SpecID = glGetUniformLocation(shaderProgram, "material.specular");
    glUniform3f(SpecID, this->material.specular.x,this->material.specular.y,this->material.specular.z);
    GLuint ShinID = glGetUniformLocation(shaderProgram, "material.shininess");
    glUniform1f(ShinID, this->material.shininess);


    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    GLuint MatrixID2 = glGetUniformLocation(shaderProgram, "MV");
    glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &PV[0][0]);
    
    if(lightType == 1)
    {
        light = glm::normalize(light);
        glm::vec4 res= Window::V * light;
        glm::normalize(res);
        GLuint LightID = glGetUniformLocation(shaderProgram, "Light");
        glUniform3fv(LightID,1,&res[0]);
    }
    else if(lightType == 2)
    {
        pl_dir = glm::normalize(pl_dir);
        glm::vec4 pl = glm::vec4(0.f,0.f,0.f,1.f)+pl_dir*pl_scale;
        glm::vec4 res= Window::V * pl;
        GLuint LightID = glGetUniformLocation(shaderProgram, "Light");
        glUniform4fv(LightID,1,&res[0]);
    }
    else
    {
        GLuint ExpID = glGetUniformLocation(shaderProgram, "spotExp");
        glUniform1f(ExpID,spotExp);
        GLuint RangeID = glGetUniformLocation(shaderProgram, "range");
        glUniform1f(RangeID,range);
        glm::vec4 d = glm::vec4(0.f,0.f,0.f,1.f);
        d = Window::V * d;
        GLuint DID = glGetUniformLocation(shaderProgram, "d");
        glUniform4fv(DID,1,&d[0]);
        spot_dir = glm::normalize(spot_dir);
        glm::vec4 ps = glm::vec4(0.f,0.f,0.f,1.f)+spot_dir*spot_scale;
        glm::vec4 res= Window::V * ps;
        GLuint LightID = glGetUniformLocation(shaderProgram, "Light");
        glUniform4fv(LightID,1,&res[0]);
    }
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLint)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
}

void OBJObject::draw(GLuint shaderProgram)
{
    // Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
    //glm::mat4 MVP = Window::P * Window::V * toWorld;
    glm::mat4 MVP = Window::P * Window::V * toWorld;
    glm::mat4 MV = Window::V * toWorld;
    glm::mat3 NM = glm::mat3(transpose(inverse(MV)));
    
    GLuint MVID = glGetUniformLocation(shaderProgram, "m_viewModel");
    glUniformMatrix4fv(MVID, 1, GL_FALSE, &MV[0][0]);
    GLuint NMID = glGetUniformLocation(shaderProgram, "NM");
    glUniformMatrix3fv(NMID, 1, GL_FALSE, &NM[0][0]);
    GLuint SpecID = glGetUniformLocation(shaderProgram, "specular");
    glUniform4fv(SpecID, 1, &(currentMat->spec[0]));
    GLuint ShinID = glGetUniformLocation(shaderProgram, "shiness");
    glUniform1f(ShinID, currentMat->shine);
    GLuint MatID = glGetUniformLocation(shaderProgram, "diffuse");
    glUniform4fv(MatID, 1, &(currentMat->mat[0]));
    GLuint AmID = glGetUniformLocation(shaderProgram, "ambient");
    glUniform4fv(AmID, 1, &(currentMat->amb[0]));
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    
    if(lightType == 1)
    {
        light = glm::normalize(light);
        glm::vec4 res= Window::V * light;
        glm::normalize(res);
        GLuint LightID = glGetUniformLocation(shaderProgram, "Light");
        glUniform3fv(LightID,1,&res[0]);
    }
    else if(lightType == 2)
    {
        pl_dir = glm::normalize(pl_dir);
        glm::vec4 pl = glm::vec4(0.f,0.f,0.f,1.f)+pl_dir*pl_scale;
        glm::vec4 res= Window::V * pl;
        GLuint LightID = glGetUniformLocation(shaderProgram, "Light");
        glUniform4fv(LightID,1,&res[0]);
    }
    else
    {
        GLuint ExpID = glGetUniformLocation(shaderProgram, "spotExp");
        glUniform1f(ExpID,spotExp);
        GLuint RangeID = glGetUniformLocation(shaderProgram, "range");
        glUniform1f(RangeID,range);
        glm::vec4 d = glm::vec4(0.f,0.f,0.f,1.f);
        d = Window::V * d;
        GLuint DID = glGetUniformLocation(shaderProgram, "d");
        glUniform4fv(DID,1,&d[0]);
        spot_dir = glm::normalize(spot_dir);
        glm::vec4 ps = glm::vec4(0.f,0.f,0.f,1.f)+spot_dir*spot_scale;
        glm::vec4 res= Window::V * ps;
        GLuint LightID = glGetUniformLocation(shaderProgram, "Light");
        glUniform4fv(LightID,1,&res[0]);
    }
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLint)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

void OBJObject::move(float x,float y)
{
    this->toWorld = (glm::translate(glm::mat4(1.f), glm::vec3(-x, y, 0.f)))*(this->toWorld);
}
void OBJObject::move(char input)
{
        switch(input){
            case 'x': this->toWorld = (glm::translate(glm::mat4(1.f), glm::vec3(-0.5f, 0.f, 0.f)))*(this->toWorld);break;
            case 'X': this->toWorld = (glm::translate(glm::mat4(1.f), glm::vec3(0.5f, 0.f, 0.f)))*(this->toWorld);break;
            case 'y': this->toWorld = (glm::translate(glm::mat4(1.f), glm::vec3(0.f, -0.5f, 0.f)))*(this->toWorld);break;
            case 'Y': this->toWorld = (glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.5f, 0.f)))*(this->toWorld);break;
            case 'z': this->toWorld = (glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -0.5f)))*(this->toWorld);break;
            case 'Z': this->toWorld = (glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.5f)))*(this->toWorld);break;
            case 's': this->toWorld = (glm::scale(this->toWorld, glm::vec3(0.5f,0.5f,0.5f)));break;
            case 'S': this->toWorld = (glm::scale(this->toWorld, glm::vec3(2.f,2.f,2.f)));break;
            case 'o': this->toWorld = (glm::rotate(glm::mat4(1.0f), 30.f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)))*(this->toWorld);break;
            case 'O': this->toWorld = (glm::rotate(glm::mat4(1.0f), -30.f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)))*(this->toWorld);break;
            case 'r': this->toWorld = glm::mat4(1.0f);break;
    }
}
void OBJObject::move(float angle,glm::vec3 axis)
{
    this->toWorld = (glm::rotate(glm::mat4(1.0f), angle, axis))*(this->toWorld);
}
/*
void OBJObject::spin(float deg)
{
    this->angle = deg;
    if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
    // This creates the matrix to rotate the object
    this->toWorld = (this->toWorld)*(glm::rotate(glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)));
}*/