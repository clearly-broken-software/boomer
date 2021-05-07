/*  license  */

#ifndef LISTVIEW_HPP
#define LISTVIEW_HPP

//#include "Window.hpp"
#include "Widget.hpp"
#include "NanoVG.hpp"
#include "fonts.hpp"
#include "filesystem.hpp"
#include <string>
#include <sstream>
#include <initializer_list>
#include <iostream>


START_NAMESPACE_DISTRHO

class ListView : public NanoWidget
{
public:
  class Callback
  {
  public:
    virtual ~Callback() {}
    virtual void onListViewClicked(ListView *listview, ghc::filesystem::path p) = 0;
  };
  explicit ListView(Window &parent) noexcept;
  explicit ListView(Widget *widget) noexcept;
  ~ListView();
    
  void addItems(std::vector<ghc::filesystem::path> &p);
  void setCallback(Callback *cb);
  ghc::filesystem::path getItem(uint index);
  void setFont(const char *name, const uchar *data, uint dataSize);
  void setMaxViewItems(int maxViewItems);

  float font_size;
  float margin;
  int timer, time_out;
  bool dropdown_has_mouse;
  int item_view_first, item_view_last;

  Color background_color;
  Color foreground_color;
  Color highlight_color;
  Color text_color;

protected:
  void onNanoDisplay() override;
  bool onMouse(const MouseEvent &) override;
  bool onMotion(const MotionEvent &) override;
  bool onScroll(const ScrollEvent &) override;

private:
  struct ListItem{
    ListItem(ghc::filesystem::path fp, std::string sp)
    {
      fullPath = fp;
      displayPath = sp;
    }
    ghc::filesystem::path fullPath;
    std::string displayPath;
  };

  void resize();
  void scrollListView(int dir);
  std::string truncatePath(ghc::filesystem::path &p, float width, bool isParent);

  std::vector<ListItem> items_;

  Callback *callback_;
  int highlighted_item_;
  bool has_mouse_;
  bool max_size_reached_;

  int max_view_items; // number of items in a ListView

  FontId listview_font_;

  DISTRHO_LEAK_DETECTOR(ListView)
};

END_NAMESPACE_DISTRHO

#endif
