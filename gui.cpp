#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "common/include.hpp"

#include "include/constants.hpp"
#include "include/globalVar.hpp"
#include "include/player.hpp"

#include "include/gui.hpp"

void initImGUI(GLFWwindow* window){
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void showF3(Player player){
    if(isF3){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("F3");

        ImGui::Text("Player position : (%.1f, %.1f, %.1f)", player.pos.x / SCALE, player.pos.y / SCALE, player.pos.z / SCALE);
        ImGui::Text("Chunk : (%.1f)", (float) player.actualChunk );

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void delImGUI(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}