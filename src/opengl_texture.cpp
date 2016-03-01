// opengl_texture.cpp
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

#include "neogfx.hpp"
#include "opengl_error.hpp"
#include "opengl_texture.hpp"

namespace neogfx
{
	opengl_texture::opengl_texture(const i_image& aImage) :
		iSize(aImage.extents()), iHandle(0), iUri(aImage.uri())
	{
		GLint previousTexture;
		try
		{
			glCheck(glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture));
			glCheck(glGenTextures(1, &iHandle));
			glCheck(glBindTexture(GL_TEXTURE_2D, iHandle));
			glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			switch (aImage.colour_format())
			{
			case ColourFormatRGBA8:
				glCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(extents().cx), static_cast<GLsizei>(extents().cy), 0, GL_RGBA, GL_UNSIGNED_BYTE, aImage.data()));
				break;
			default:
				throw unsupported_colour_format();
				break;
			}
			glCheck(glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture)));
		}
		catch (...)
		{
			glCheck(glDeleteTextures(1, &iHandle));
			throw;
		}
	}

	opengl_texture::~opengl_texture()
	{
		glCheck(glDeleteTextures(1, &iHandle));
	}

	const size& opengl_texture::extents() const
	{
		return iSize;
	}

	void* opengl_texture::handle() const
	{
		return reinterpret_cast<void*>(iHandle);
	}

	const std::string& opengl_texture::uri() const
	{
		return iUri;
	}
}