#include "ui/ui_sizer.h"
#include "ui/ui_widget.h"

using namespace Halley;

UISizerEntry::UISizerEntry()
{
}

UISizerEntry::UISizerEntry(UIElementPtr widget, float proportion, Vector4f border, int fillFlags)
	: widget(widget)
	, proportion(proportion)
	, border(border)
	, fillFlags(fillFlags)
{
}

float UISizerEntry::getProportion() const
{
	return proportion;
}

Vector2f UISizerEntry::getMinimumSize() const
{
	return widget ? widget->computeMinimumSize() : Vector2f();
}

void UISizerEntry::placeInside(Rect4f rect, Vector2f minSize)
{
	Vector2f cellSize = rect.getSize();
	Vector2f anchoring;
	Vector2f size = minSize;
	
	if (fillFlags & UISizerAlignFlags::Top) {
		anchoring.y = 0.0f;
	}
	if (fillFlags & UISizerAlignFlags::CentreVertical) {
		anchoring.y = 0.5f;
	}
	if (fillFlags & UISizerAlignFlags::Bottom) {
		anchoring.y = 1.0f;
	}
	if (fillFlags & UISizerAlignFlags::Left) {
		anchoring.x = 0.0f;
	}
	if (fillFlags & UISizerAlignFlags::CentreHorizontal) {
		anchoring.x = 0.5f;
	}
	if (fillFlags & UISizerAlignFlags::Right) {
		anchoring.x = 1.0f;
	}
	if (fillFlags & UISizerFillFlags::FillHorizontal) {
		size.x = cellSize.x;
	}
	if (fillFlags & UISizerFillFlags::FillVertical) {
		size.y = cellSize.y;
	}

	Vector2f spareSize = cellSize - size;
	Vector2f pos = (rect.getTopLeft() + spareSize * anchoring).round();

	if (widget) {
		widget->setRect(Rect4f(pos, pos + size));
	}
}

UIElementPtr UISizerEntry::getPointer() const
{
	return widget;
}

Vector4f UISizerEntry::getBorder() const
{
	return border;
}

int UISizerEntry::getFillFlags() const
{
	return fillFlags;
}

UISizer::UISizer(UISizerType type, float gap)
	: type(type)
	, gap(gap)
{
}

Vector2f UISizer::computeMinimumSize() const
{
	return computeMinimumSize(true);
}

Vector2f UISizer::computeMinimumSize(bool includeProportional) const
{
	float totalProportion = 0;
	for (auto& e: entries) {
		totalProportion += e.getProportion();
	}

	int mainAxis = type == UISizerType::Horizontal ? 0 : 1;
	int otherAxis = 1 - mainAxis;
	
	float main = 0;
	float biggestProportional = 0;
	float other = 0;
	
	bool first = true;
	for (auto& e: entries) {
		Vector2f sz = e.getMinimumSize();
		auto border = e.getBorder();
		if (!first) {
			border[mainAxis] += gap;
		}
		first = false;
		
		other = std::max(other, sz[otherAxis] + border[otherAxis] + border[otherAxis + 2]);

		float p = e.getProportion();
		if (p > 0.0001f) {
			float s = sz[mainAxis] / p;
			biggestProportional = std::max(biggestProportional, s);
		} else {
			main += sz[mainAxis];
		}

		main += border[mainAxis] + border[mainAxis + 2];
	}
	if (includeProportional) {
		main += biggestProportional * totalProportion;
	}

	Vector2f result;
	result[mainAxis] = main;
	result[otherAxis] = other;
	return result;
}

void UISizer::setRect(Rect4f rect)
{
	Vector2f pos = rect.getTopLeft();

	float totalProportion = 0;
	for (auto& e: entries) {
		totalProportion += e.getProportion();
	}

	int mainAxis = type == UISizerType::Horizontal ? 0 : 1;
	int otherAxis = 1 - mainAxis;

	Vector2f sizerMinSize = computeMinimumSize(false);
	float spare = (rect.getSize() - sizerMinSize)[mainAxis];
	
	bool first = true;
	for (auto& e: entries) {
		float p = e.getProportion();
		auto border = e.getBorder();
		if (!first) {
			border[mainAxis] += gap;
		}
		first = false;

		Vector2f minSize = e.getMinimumSize();
		Vector2f cellSize = minSize;
		if (p > 0.0001f) {
			float propSize = std::floor(spare * p / totalProportion);
			spare -= propSize;
			totalProportion -= p;
			cellSize[mainAxis] = propSize;
		}
		cellSize[otherAxis] = rect.getSize()[otherAxis] - border[otherAxis] - border[otherAxis + 2];

		Vector2f curPos = pos + Vector2f(border.x, border.y);
		e.placeInside(Rect4f(curPos, curPos + cellSize), minSize);

		pos[mainAxis] += cellSize[mainAxis] + border[mainAxis] + border[mainAxis + 2];
	}
}

void UISizer::add(std::shared_ptr<UIWidget> widget, float proportion, Vector4f border, int fillFlags)
{
	addElement(widget, proportion, border, fillFlags);
}

void UISizer::add(std::shared_ptr<UISizer> sizer, float proportion, Vector4f border, int fillFlags)
{
	addElement(sizer, proportion, border, fillFlags);
}

void UISizer::addSpacer(float size)
{
	entries.emplace_back(UISizerEntry({}, 0, Vector4f(type == UISizerType::Horizontal ? size : 0.0f, type == UISizerType::Vertical ? size : 0.0f, 0.0f, 0.0f), {}));
}

void UISizer::addStretchSpacer(float proportion)
{
	entries.emplace_back(UISizerEntry({}, proportion, {}, {}));
}

void UISizer::reparent(UIParent& parent)
{
	for (auto& e: entries) {
		auto widget = std::dynamic_pointer_cast<UIWidget>(e.getPointer());
		if (widget) {
			parent.addChild(widget);
		} else {
			auto sizer = std::dynamic_pointer_cast<UISizer>(e.getPointer());
			if (sizer) {
				sizer->reparent(parent);
			}
		}
	}
}

void UISizer::addElement(UIElementPtr widget, float proportion, Vector4f border, int fillFlags)
{
	entries.emplace_back(UISizerEntry(widget, proportion, border, fillFlags));
}

UISizerType UISizer::getType() const
{
	return type;
}

size_t UISizer::size() const
{
	return entries.size();
}

const UISizerEntry& UISizer::operator[](size_t n) const
{
	return entries[n];
}

UISizerEntry& UISizer::operator[](size_t n)
{
	return entries[n];
}

void UISizer::clear()
{
	for (auto& e: entries) {
		auto widget = std::dynamic_pointer_cast<UIWidget>(e.getPointer());
		if (widget) {
			widget->destroy();
		} else {
			auto sizer = std::dynamic_pointer_cast<UISizer>(e.getPointer());
			if (sizer) {
				sizer->clear();
			}
		}
	}
	entries.clear();
}