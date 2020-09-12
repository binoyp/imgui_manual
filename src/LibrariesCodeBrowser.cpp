#include "LibrariesCodeBrowser.h"
#include "imgui_utilities/MarkdownHelper.h"
#include "imgui_utilities/ImGuiExt.h"
#include "hello_imgui/hello_imgui.h"
#include <fplus/fplus.hpp>

#ifdef HELLOIMGUI_HAS_OPENGL
void DrawImage_FixedWith(const HelloImGui::ImageGlPtr& image, float width)
{
    float ratio = width / image->imageSize.x;
    ImVec2 size(image->imageSize.x * ratio, image->imageSize.y * ratio);
    ImGui::Image(image->imTextureId, size);
}
#endif

LibrariesCodeBrowser::LibrariesCodeBrowser(
    const std::string & windowName,
    const std::vector<SourceParse::Library> &librarySources,
    std::string currentSourcePath)
        : WindowWithEditor(windowName)
        , mLibraries(librarySources)
{
    if (!currentSourcePath.empty())
        mCurrentSource = SourceParse::ReadSource(currentSourcePath);
    mEditor.SetText(mCurrentSource.sourceCode);
}

void LibrariesCodeBrowser::gui()
{
    if (guiSelectLibrarySource())
        mEditor.SetText(mCurrentSource.sourceCode);

    std::string sourcePath = mCurrentSource.sourcePath;
    if (fplus::is_suffix_of(std::string(".md"), sourcePath))
        MarkdownHelper::Markdown(mCurrentSource.sourceCode);
    else if (fplus::is_suffix_of(std::string(".png"), sourcePath))
    {
#ifdef HELLOIMGUI_HAS_OPENGL
        if (mTextureCache.find(sourcePath) == mTextureCache.end())
        {
            std::string assetPath = std::string("code/") + sourcePath.c_str();
            mTextureCache[sourcePath] = HelloImGui::ImageGl::FactorImage(assetPath.c_str());
        }

        DrawImage_FixedWith(mTextureCache[sourcePath], ImGui::GetWindowSize().x - 30.f);
#endif
    }
    else
        RenderEditor(mCurrentSource.sourcePath.c_str());
}

bool LibrariesCodeBrowser::guiSelectLibrarySource()
{
    bool changed = false;
    for (const auto & librarySource: mLibraries)
    {
        ImGui::Text("%s", librarySource.name.c_str());
        ImGui::SameLine(ImGui::GetWindowSize().x - 350.f );
        ImGuiExt::Hyperlink(librarySource.url);
        MarkdownHelper::Markdown(librarySource.shortDoc);
        for (const auto & source: librarySource.sourcePaths)
        {
            std::string currentSourcePath = librarySource.path + "/" + source;
            bool isSelected = (currentSourcePath == mCurrentSource.sourcePath);
            std::string buttonLabel = source + "##" + librarySource.path;
            if (isSelected)
                ImGui::TextDisabled("%s", source.c_str());
            else if (ImGui::Button(buttonLabel.c_str()))
            {
                mCurrentSource = SourceParse::ReadSource(currentSourcePath);
                changed = true;
            }
            ImGuiExt::SameLine_IfPossible(150.f);
        }
        ImGui::NewLine();
        ImGui::Separator();
    }
    return changed;
}
