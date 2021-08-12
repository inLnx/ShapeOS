/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <base/Noncopyable.h>
#include <base/String.h>
#include <base/Vector.h>
#include <libvt/Attribute.h>
#include <libvt/Position.h>
#include <libvt/XtermColors.h>

namespace VT {

class Line {
    BASE_MAKE_NONCOPYABLE(Line);
    BASE_MAKE_NONMOVABLE(Line);

public:
    explicit Line(size_t length);
    ~Line();

    struct Cell {
        u32 code_point { ' ' };
        Attribute attribute;

        bool operator!=(Cell const& other) const { return code_point != other.code_point || attribute != other.attribute; }
    };

    const Attribute& attribute_at(size_t index) const { return m_cells[index].attribute; }
    Attribute& attribute_at(size_t index) { return m_cells[index].attribute; }

    Cell& cell_at(size_t index) { return m_cells[index]; }
    const Cell& cell_at(size_t index) const { return m_cells[index]; }

    void clear(const Attribute& attribute = Attribute())
    {
        m_terminated_at.clear();
        clear_range(0, m_cells.size() - 1, attribute);
    }
    void clear_range(size_t first_column, size_t last_column, const Attribute& attribute = Attribute());
    bool has_only_one_background_color() const;

    bool is_empty() const
    {
        return !any_of(m_cells, [](auto& cell) { return cell != Cell(); });
    }

    size_t length() const
    {
        return m_cells.size();
    }
    void set_length(size_t);
    void rewrap(size_t new_length, Line* next_line, CursorPosition* cursor, bool cursor_is_on_next_line = true);

    u32 code_point(size_t index) const
    {
        return m_cells[index].code_point;
    }

    void set_code_point(size_t index, u32 code_point)
    {
        if (m_terminated_at.has_value()) {
            if (index > *m_terminated_at) {
                m_terminated_at = index + 1;
            }
        }

        m_cells[index].code_point = code_point;
    }

    bool is_dirty() const { return m_dirty; }
    void set_dirty(bool b) { m_dirty = b; }

    Optional<u16> termination_column() const { return m_terminated_at; }
    void set_terminated(u16 column) { m_terminated_at = column; }

private:
    void take_cells_from_next_line(size_t new_length, Line* next_line, bool cursor_is_on_next_line, CursorPosition* cursor);
    void push_cells_into_next_line(size_t new_length, Line* next_line, bool cursor_is_on_next_line, CursorPosition* cursor);

    Vector<Cell> m_cells;
    bool m_dirty { false };

    [[no_unique_address]] Optional<u16> m_terminated_at;
};

}
