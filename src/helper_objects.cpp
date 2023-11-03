#include "helper_objects.hpp"

void BasicCube::shift(const glm::vec3& v) {
	m_model = glm::translate(m_model, v);
	m_coords += v;
}

void BasicCube::scale(const glm::vec3& v) {
	m_model = glm::scale(m_model, v);
}

void BasicCube::rotate(float theta, const glm::vec3& axis) {
	m_model = glm::rotate(m_model, glm::radians(theta), axis);
}

void BasicCube::move_to(const glm::vec3& coords) {
	shift(coords - m_coords);
}

glm::vec3 BasicCube::coordinates() { return m_coords; };

BasicCube::BasicCube(const glm::vec3& coords) : m_model(1.0f), m_coords(coords) {
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	shift(coords);
}
BasicCube::BasicCube() : BasicCube({0.0f, 0.0f, 0.0f}) {}

void BasicCube::draw(glm::mat4 projection, glm::mat4 view, unsigned int texture_id, Shader& shader) {
	glBindVertexArray(cubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	shader.use();
	glUniform1i(glGetUniformLocation(shader.ID, "texture_diffuse1"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(m_model));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
}
