/**
 * \file Actor.cpp
 *
 * \author Charles Bean
 */

#include "stdafx.h"
#include "Actor.h"
#include "Picture.h"

/** \brief Non-default constructor
* \param name - A string to set the actors name as */
CActor::CActor(const wstring &name) : mName(name)
{

}

/** \brief Virtual Destructor */
CActor::~CActor()
{
}

/** \brief Set the root drawable for the actor
* \param root Pointer to root drawable */
void CActor::SetRoot(std::shared_ptr<CDrawable> root)
{
	mRoot = root;
}

/** \brief Draws our actor
* \param graphics - A graphics context (pointer) to draw upon */
void CActor::Draw(Graphics *graphics)
{
	// Don't draw if not enabled
	if (!mEnabled)
		return;

	// This takes care of determining the absolute placement
	// of all of the child drawables. We have to determine this 
	// in tree order, which may not be the order we draw.
	if (mRoot != nullptr)
		mRoot->Place(mPosition, 0);

	for (auto drawable : mDrawablesInOrder)
	{
		drawable->Draw(graphics);
	}
}

/** \brief A hit test for our actor
* \param pos - A point to test
* \return The object (CDrawable) that is hit */
shared_ptr<CDrawable> CActor::HitTest(Point pos)
{
	// If not enabled or not clickable, we indicate no hit.
	if (!mClickable || !mEnabled)
		return nullptr;

	// Since this list is in drawing order, we realy want to know the last thing drawn
	// under the mouse, since it will be on top. So, we reverse iterate over the list.
	for (auto d = mDrawablesInOrder.rbegin(); d != mDrawablesInOrder.rend(); d++)
	{
		auto drawable = *d;
		if (drawable->HitTest(pos))
			return drawable;
	}

	return nullptr;
}

/** \brief Adds a drawable to our actor
* \param drawable - The CDrawable object to add to our actor */
void CActor::AddDrawable(shared_ptr<CDrawable> drawable)
{
	mDrawablesInOrder.push_back(drawable);
	drawable->SetActor(this);
}

/** \brief Set the picture link for this actor.
*
* This is telling the actor what
* picture to use.
*
* Also tells all child drawables what the timeline is.
* \param picture The picture we are using.
*/
void CActor::SetPicture(CPicture *picture)
{
	mPicture = picture;

	// Add the channel
	mPicture->GetTimeline()->AddChannel(&mPositionChannel);

	// Set the timeline for all drawables. This links the channels to
	// the timeline system.
	for (auto drawable : mDrawablesInOrder)
	{
		drawable->SetTimeline(mPicture->GetTimeline());
	}

}

/** \brief Set a keyframe on an actor.
*/
void CActor::SetKeyframe()
{
	mPositionChannel.SetKeyframe(mPosition);

	for (auto drawable : mDrawablesInOrder)
	{
		drawable->SetKeyframe();
	}
}

/** brief Get a keyframe for an actor.
*/
void CActor::GetKeyframe()
{
	if (mPositionChannel.IsValid())
		mPosition = mPositionChannel.GetPoint();

	for (auto drawable : mDrawablesInOrder)
	{
		drawable->GetKeyframe();
	}
}