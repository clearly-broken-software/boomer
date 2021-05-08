/*
* 
*/
#include "BoomerUI.hpp"

START_NAMESPACE_DISTRHO

BoomerUI::BoomerUI()
    : UI(1000, 600), fScale(1.0f)
{
    // char *p = getenv("HOME");
    // if (p)
    //     printf("%s\n", p);
    midiDir = ghc::filesystem::current_path();
   
    fSampleRate = getSampleRate();
    fFont = createFontFromFile("sans", "/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf");

    fListView.reset(new ListView(this));
    //fListView = new ListView(this);
    fListView->background_color = black_olive;
    fListView->foreground_color = black_olive_2;
    fListView->highlight_color = flame_2;
    fListView->text_color = floral_white;
    fListView->setCallback(this);
    fListView->setId(42);
    fListView->setMaxViewItems(32);
    fListView->setSize(200, (32) * fListView->font_size);
    fListView->setAbsolutePos(10, 10);
    fListView->setFont("Roboto_Regular",
                       reinterpret_cast<const uchar *>(fonts::Roboto_RegularData),
                       fonts::Roboto_RegularDataSize);
    fListView->show();

    std::vector<fs::path> tree;
    makeTree(midiDir, tree);
    fListView->addItems(tree);
}

void BoomerUI::parameterChanged(uint32_t , float )
{
    // do stuff
  }

void BoomerUI::sampleRateChanged(double )
{
   // do stuff
  
}
void BoomerUI::stateChanged(const char *, const char *)
{
    // do stuff
}

void BoomerUI::onNanoDisplay()
{
    // do stuff
}

void BoomerUI::makeTree(fs::path filePath, std::vector<fs::path> &tree)
{
    std::vector<fs::path> tmp;
    tree.push_back(filePath);               // current path
    tree.push_back(filePath.parent_path()); // display as ../
    for (auto &p : fs::directory_iterator(filePath))
    {
        std::string fn = p.path().filename();
        if (fn[0] == '.')
        {
            continue;
        } // add dirs
        if (fs::is_directory(p))
        {
            tmp.push_back(p.path());
            continue;
        }
    }
    std::sort(tmp.begin(), tmp.end());
    tree.insert(tree.end(), tmp.begin(), tmp.end());
    tmp.clear();

    // only add .mid files
    for (auto &p : fs::directory_iterator(filePath))
    {
        // get extension
        std::string e = p.path().extension();
        // convert string to lower case
        std::for_each(e.begin(), e.end(), [](char &c) {
            c = ::tolower(c);
        });
        if (e == ".mid")
            tmp.push_back(p.path());
    }
    std::sort(tmp.begin(), tmp.end());
    tree.insert(tree.end(), tmp.begin(), tmp.end());
}

void BoomerUI::onListViewClicked(ListView *lv, fs::path item)
{
    fs::path p = item;
    if (fs::is_directory(p))
    {
        midiDir = p;
        std::vector<fs::path> tree;
        makeTree(p, tree);
        fListView->addItems(tree);
        repaint();
    }
    else
    {
        setState("midifile", item.c_str());
        //printf("item = %s\n", item.c_str());
    }
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
