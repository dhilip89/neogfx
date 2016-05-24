// text_edit.hpp
/*
  neogfx C++ GUI Library
  Copyright(C) 2016 Leigh Johnston
  
  This program is free software: you can redistribute it and / or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "neogfx.hpp"
#include <neolib/tag_array.hpp>
#include <neolib/segmented_array.hpp>
#include "scrollable_widget.hpp"
#include "i_document.hpp"
#include "glyph.hpp"
#include "cursor.hpp"

namespace neogfx
{
	class text_edit : public scrollable_widget, public i_document
	{
	public:
		class style
		{
		public:
			typedef neolib::variant<colour, gradient> colour_type;
		public:
			style();
			style(
				const optional_font& aFont,
				const colour_type& aTextColour,
				const colour_type& aBackgroundColour);
			style(
				text_edit& aParent,
				const style& aOther);
		public:
			void add_ref() const;
			void release() const;
		public:
			const optional_font& font() const;
			const colour_type& text_colour() const;
			const colour_type& background_colour() const;
		public:
			bool operator<(const style& aRhs) const;
		private:
			text_edit* iParent;
			mutable uint32_t iUseCount;
			optional_font iFont;
			colour_type iTextColour;
			colour_type iBackgroundColour;
		};
		typedef std::set<style> style_list;
	private:
		struct unknown_node {};
		template <typename Node = unknown_node>
		class tag
		{
		public:
			typedef neolib::variant<style_list::const_iterator> contents_type;
			template <typename Node2>
			struct rebind { typedef tag<Node2> type; };
		private:
			typedef Node node_type;
		public:
			tag(const contents_type& aContents) :
				iNode(nullptr), iContents(aContents)
			{
				if (iContents.is<style_list::const_iterator>())
					static_variant_cast<style_list::const_iterator>(iContents)->add_ref();
			}
			template <typename Node2>
			tag(node_type& aNode, const tag<Node2>& aTag) : 
				iNode(&aNode), iContents(aTag.iContents)
			{
				if (iContents.is<style_list::const_iterator>())
					static_variant_cast<style_list::const_iterator>(iContents)->add_ref();
			}
			tag(const tag& aOther) : 
				iNode(aOther.iNode), iContents(aOther.iContents)
			{
				if (iContents.is<style_list::const_iterator>())
					static_variant_cast<style_list::const_iterator>(iContents)->add_ref();
			}
			~tag()
			{
				if (iContents.is<style_list::const_iterator>())
					static_variant_cast<style_list::const_iterator>(iContents)->release();
			}
		public:
			bool operator==(const tag& aOther) const
			{
				return iContents == aOther.iContents;
			}
			bool operator!=(const tag& aOther) const
			{
				return !(*this == aOther);
			}
		public:
			const contents_type& contents() const
			{
				return iContents;
			}
		private:
			node_type* iNode;
			contents_type iContents;
		};
		typedef neolib::tag_array<tag<>, char, 256> document_text;
		typedef neolib::segmented_array<glyph, 256> document_glyphs;
	public:
		typedef document_text::size_type position_type;
	public:
		text_edit();
		text_edit(i_widget& aParent);
		text_edit(i_layout& aLayout);
		~text_edit();
	public:
		virtual size minimum_size(const optional_size& aAvailableSpace = optional_size()) const;
	public:
		virtual void paint(graphics_context& aGraphicsContext) const;
	public:
		virtual bool key_pressed(scan_code_e aScanCode, key_code_e aKeyCode, key_modifiers_e aKeyModifiers);
		virtual bool key_released(scan_code_e aScanCode, key_code_e aKeyCode, key_modifiers_e aKeyModifiers);
		virtual bool text_input(const std::string& aText);
	public:
		virtual void move_cursor(cursor::move_operation_e aMoveOperation) const;
	public:
		neogfx::alignment alignment() const;
		void set_alignment(neogfx::alignment aAlignment);
		const style& default_style() const;
		void set_default_style(const style& aDefaultStyle);
		colour default_text_colour() const;
	public:
		neogfx::cursor& cursor() const;
		point position(position_type aPosition) const;
		position_type hit_test(const point& aPoint) const;
		std::string text() const;
		void set_text(const std::string& aText);
		void set_text(const std::string& aText, const style& aStyle);
		void insert_text(const std::string& aText);
		void insert_text(const std::string& aText, const style& aStyle);
	private:
		void init();
		void refresh_paragraph(document_text::const_iterator aWhere);
		void animate();
		void draw_glyph_text(const graphics_context& aGraphicsContext, const point& aPoint, document_glyphs::const_iterator aTextBegin, document_glyphs::const_iterator aTextEnd) const;
		size extents(const neogfx::font& aFont, document_glyphs::const_iterator aBegin, document_glyphs::const_iterator aEnd) const;
		std::pair<document_glyphs::const_iterator, document_glyphs::const_iterator> word_break(document_glyphs::const_iterator aBegin, document_glyphs::const_iterator aFrom) const;
	private:
		neogfx::alignment iAlignment;
		style iDefaultStyle;
		mutable neogfx::cursor iCursor;
		document_text iText;
		document_glyphs iGlyphs;
		style_list iStyles;
		neolib::callback_timer iAnimator;
	};
}