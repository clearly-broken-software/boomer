/* license */
#include "ListView.hpp"

START_NAMESPACE_DISTRHO

ListView::ListView(Window &parent) noexcept
    : NanoWidget(parent)
{
    highlighted_item_ = -1;
    font_size = 14;
    margin = 2.f;
    timer = 0; // if >= 0 listview is visible
    time_out = 150;
    max_view_items = 4;
    item_view_first = 0;
    item_view_last = max_view_items - 1;
    has_mouse_ = false;
    dropdown_has_mouse = false;
    listview_font_ = 0;
    background_color = Color(0, 0, 0);
    foreground_color = Color(255, 255, 255);
    highlight_color = Color(255, 0, 0);
}
ListView::ListView(Widget *widget) noexcept
    : NanoWidget(widget)
{
    highlighted_item_ = -1;
    font_size = 14;
    margin = 2.f;
    timer = 0; // if >= 0 listview is visible
    time_out = 150;
    max_view_items = 4;
    item_view_first = 0;
    item_view_last = max_view_items - 1;
    has_mouse_ = false;
    dropdown_has_mouse = false;
    listview_font_ = 0;
    background_color = Color(0, 0, 0);
    foreground_color = Color(255, 255, 255);
    highlight_color = Color(255, 0, 0);
}

ListView::~ListView()
{
    printf("ListView destroyed");
}

bool ListView::onMouse(const MouseEvent &ev)
{
    if (ev.press && ev.button == 1)
    {
        if (contains(ev.pos))
        {
            if (highlighted_item_ >= 0)
            {
                const uint index = highlighted_item_ + item_view_first;
                callback_->onListViewClicked(this, items_[index].fullPath);
                return true;
            }
            if (ev.pos.getY() < font_size)
            {
                scrollListView(-1); // scroll back
                return false;
            }
            if (ev.pos.getY() > getHeight() - font_size)
            {
                scrollListView(1);
                return false;
            }
        }
    }
    return false;
}

bool ListView::onMotion(const MotionEvent &ev)
{
    if (!isVisible())
        return false;
    if (contains(ev.pos))
    {
        int offset_y = 0;
        has_mouse_ = true;
        const int mouse_y = ev.pos.getY();
        highlighted_item_ = mouse_y / font_size;
        highlighted_item_ -= offset_y;
        if (highlighted_item_ > item_view_last || highlighted_item_ > items_.size() - 1)
        {
            highlighted_item_ = -1;
        }
        repaint();
        return true;
    }
    else
    {
        has_mouse_ = false;
        if (highlighted_item_ >= 0)
        {
            highlighted_item_ = -1;
            repaint();
        }
    }

    return true;
}

bool ListView::onScroll(const ScrollEvent &ev)
{
    if (!isVisible())
        return false;
    if (!contains(ev.pos))
        return false;
    if (max_size_reached_)
    {
        const int delta = -static_cast<int>(ev.delta.getY());
        scrollListView(delta);
    }
    return true;
}

void ListView::onNanoDisplay()
{
    int width = getWidth();
    int height = getHeight();

    fontFaceId(listview_font_);
    beginPath();
    fillColor(background_color);
    rect(0, 0, width, height);
    fill();
    stroke();
    closePath();
    float y_offset = 0;
    // no arrows, we'll use scrollbar

    fontSize(font_size);
    if (highlighted_item_ >= 0)
    {
        beginPath();
        rect(0, font_size * highlighted_item_ + y_offset, width, font_size);
        fillColor(highlight_color);
        fill();
        closePath();
    }
    int j = 0;
    textAlign(ALIGN_TOP);
    int max_i = std::min(static_cast<int>(items_.size()), max_view_items);

    // 1st item is parent dir , truncate this
    // items[0] == parent dir, always draw this
    beginPath();
    fillColor(text_color);
    text(0, margin, items_[0].displayPath.c_str(), nullptr);
    closePath();
    // draw line
    beginPath();
    strokeWidth(2);
    strokeColor(highlight_color);
    moveTo(0, font_size + margin);
    lineTo(width, font_size + margin);
    stroke();
    closePath();
    // rest of the items
    for (int i = 1; i < max_i; i++)
    {
        beginPath();
        j = i + item_view_first;
        if (j == highlighted_item_ + item_view_first)
        {
            fillColor(background_color);
        }
        else
        {
            fillColor(text_color);
        }

        // list dirs , use dir colors
        // list files , use file colors

        text(0, font_size * i + margin + y_offset,
             items_[j].displayPath.c_str(), nullptr);
        closePath();
    }
}

void ListView::setCallback(Callback *cb)
{
    callback_ = cb;
}

std::string ListView::truncatePath(ghc::filesystem::path &p, float width, bool isParent)
{
    std::string shortPath = p.string();
    if (isParent)
    {
        Rectangle<float> bounds;
        int longPathLength = p.string().length();
        for (int pos = 0; pos < longPathLength; pos++)
        {
            textBounds(0, 0, shortPath.c_str(), NULL, bounds);
            // too large ?
            if (bounds.getWidth() > width)
            {
                while (pos < longPathLength)
                {
                    unsigned char byte = static_cast<unsigned char>(p.string()[pos]);
                    if ((byte & 0xC0) != 0x80)
                        break;
                    pos++;
                }
                shortPath = p.string().substr(pos);
            }
            else
                break;
        }
        return shortPath;
    }

    return p.filename().string();
}

void ListView::addItems(std::vector<ghc::filesystem::path> &p)
{
    float w = getWidth();
    items_.clear();
    // 1st item is current dir

    // curren dir
    std::string shortPath = truncatePath(p[0], w, true);
    items_.emplace_back(ListItem(p[0], shortPath));
    // parent dir
    shortPath = "../";
    items_.emplace_back(ListItem(p[1], shortPath));
    // dirs and files
    for (int i = 2; i < p.size(); i++)
    {

        std::string shortPath = truncatePath(p[i], w, false);
        items_.emplace_back(ListItem(p[i], shortPath));
    }
    max_size_reached_ = static_cast<int>(items_.size()) > max_view_items ? true : false;
}

void ListView::setMaxViewItems(int maxViewItems)
{
    max_view_items = maxViewItems;
    item_view_last = max_view_items - 1;
}

void ListView::resize()
{

    // measure total size
    float bounds[4]; // xmin,ymin xmax,ymax
    std::stringstream buffer;
    for (auto i : items_)
    {
        buffer << i.displayPath << "\n";
    }
    beginPath();
    fontSize(font_size);
    textAlign(ALIGN_LEFT | ALIGN_TOP);
    textBoxBounds(0.f, 0.f, 200.f, buffer.str().c_str(), nullptr, bounds);
    closePath();

    const float width = bounds[2] - bounds[0];
    float height = bounds[3] - bounds[1];

    if (max_size_reached_) // add up and down 'arrow'
        height = max_view_items * (font_size) + (2 * font_size);

    setSize(width, height);
}

void ListView::scrollListView(int dir)
{
    item_view_first += dir;
    const int upper = items_.size() - max_view_items;
    const int lower = 0;
    item_view_first = std::min(upper, std::max(item_view_first, lower));
    repaint();
}

ghc::filesystem::path ListView::getItem(uint index)
{
    return items_[index].fullPath;
}

void ListView::setFont(const char *fontName, const uchar *data, uint dataSize)
{
    listview_font_ = createFontFromMemory(fontName, data, dataSize, false);
}

END_NAMESPACE_DISTRHO
