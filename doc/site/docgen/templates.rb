def md(string)
  string ||= ""
  string = string.gsub('"', '\"').split("\n").join('" "')
  %Q({{< md "#{string}" >}})
end

def h(dom_level, name, ref, klass=nil)
  dom_level = dom_level + 2

  <<~EOF
    <h#{dom_level} #{"class=#{klass}" if klass}" id="#{ref}">
    #{name}
    <span class="hx-absolute -hx-mt-20"></span>
    <a href="##{ref}" class="subheading-anchor" aria-label="Permalink for this section"></a>
    </h#{dom_level}>
  EOF
end

class Member < OpenStruct
  @@enum_template = ERB.new <<~'EOF'
    <div class="hx-grid hx-gap-2 grid-cols-2 mb-1 mt-1 align-center">
      <%= h(dom_level, full_name, ref, "align-center") %><em class="hx-text-sm"><%= md(summary) %></em>
    </div>

    <%= md short_description %>

    <% if deprecated %>

    Deprecated <%= deprecated %>
    <% end %>
    <% if see %>

    See <%= see %>
    <% end %>

    <dl class="inline">
    <%= generate_members() %>
    </dl>
  EOF

  @@fn_template = ERB.new <<~'EOF'
    <div class="hx-grid hx-gap-2 grid-cols-2 mb-1 mt-1 align-center">
      <%= h(dom_level, full_name, ref, "align-center") %><em class="hx-text-sm"><%= "(overload #{overload_index}) " if overload_index %><%= md(summary) %></em>
    </div>

    ```lua
    <%= (returns.map {|p| p["name"] }.join(", ") + " = ") unless returns.reject {|r| r["name"].nil? or r["name"].empty? }.empty?  %><%= full_name %>(<%= params.map {|p| p["name"] }.join(", ") unless params.empty? %>)
    ```

    <%= md short_description %>
    <% if deprecated %>

    Deprecated <%= deprecated %>
    <% end %>
    <% if see %>

    See <%= see %>
    <% end %>

    <%= h(dom_level + 1, "Arguments", "#{ref}_arguments") %>

    <% if params.empty? %>
      No arguments
    <% else %>
    <dl>
    <% params.each do |param| %>
      <dt><%= param["name"] %></dt>
      <dd>
        <code><%= param["typeref"] %></code> <%= md(param["desc"]) %>
      </dd>
    <% end %>
    </dl>
    <% end %>

    <%= h(dom_level + 1, "Returns", "#{ref}_returns") %>

    <% if returns.empty? %>
      No return value (`nil`)
    <% else %>
    <dl>
    <% returns.each do |return_value| %>
      <dt><%= return_value["name"] %></dt>
      <dd>
        <code><%= return_value["typeref"] %></code> <%= md(return_value["desc"]) %>
      </dd>
    <% end %>
    </dl>
    <% end %>
  EOF

  @@table_template = ERB.new <<~'EOF'
    <div class="hx-grid hx-gap-2 grid-cols-2 mb-1 mt-1 align-center">
      <%= h(dom_level, full_name, ref, "align-center") %><em class="hx-text-sm">(<%= type %>) <%= md(summary) %></em>
    </div>

    <%= md short_description %>
    <% if deprecated %>

    Deprecated <%= deprecated %>
    <% end %>
    <% if see %>

    See <%= see %>
    <% end %>

    <% if members.empty? %>
    No members
    <% else %>
    <%= h(dom_level + 1, "Members", "#{ref}_members") %>

    <%= generate_members() %>
    <% end %>
  EOF

  @@class_template = ERB.new <<~'EOF'
    <div class="hx-grid hx-gap-2 grid-cols-2 mb-1 mt-1 align-center">
      <%= h(dom_level, full_name, ref, "align-center") %><em class="hx-text-sm">(<%= type %>) <%= md(summary) %></em>
    </div>

    <%= md short_description %>
    <% if deprecated %>

    Deprecated <%= deprecated %>
    <% end %>
    <% if see %>

    See <%= see %>
    <% end %>

    <%= h(dom_level + 1, "Members", "#{ref}_members") %>

    <% if members.empty? %>
    No members
    <% else %>
    <dl><%= generate_members() %></dl>
    <% end %>
  EOF

  @@global_template = ERB.new <<~'EOF'
    <div class="hx-grid hx-gap-2 grid-cols-2 mb-1 mt-1 align-center">
      <%= h(dom_level, full_name, ref, "align-center") %><em class="hx-text-sm">(<%= type %>) <%= md(summary) %></em>
    </div>

    <%= md short_description %>
    <% if deprecated %>

    Deprecated <%= deprecated %>
    <% end %>
    <% if see %>

    See <%= see %>
    <% end %>

    <%= h(dom_level + 1, "Members", "#{ref}_members") %>

    <% if members.empty? %>
    No members
    <% else %>
    <dl><%= generate_members() %></dl>
    <% end %>
  EOF

  @@alias_template = ERB.new <<~'EOF'
    <dt id="<%= ref %>"><a href="#<%= ref %>"><%= full_name %></a></dt>
    <dd><%= typeref %></dd>
  EOF

  @@dunno_template = ERB.new <<~'EOF'
    <div class="hx-grid hx-gap-2 grid-cols-2 mb-1 mt-1 align-center">
      <%= h(dom_level, full_name, ref, "align-center") %><em class="hx-text-sm">(<%= type %>) <%= md(summary) %></em>
    </div>

    <%= md short_description %>
    <% if deprecated %>

    Deprecated <%= deprecated %>
    <% end %>
    <% if see %>

    See <%= see %>
    <% end %>
  EOF

  @@definition_template = ERB.new <<~'EOF'
    <% if type == :alias || custom["helper"] %>
    <% else %>
    <div class="hx-grid hx-gap-2 grid-cols-2 mb-1 mt-1 align-baseline">
      <h3 id="<%= ref %>-heading" data-notoc="">
        <a href="#<%= ref %>"><%= name %> <% if overload_index %> <em class="hx-text-sm">(overload <%= overload_index %>)</em> <% end %></a>
      </h3>
      <p><%= md(summary) %></p>
    </div>

    <% if type != :enum && !custom["no_toc_contents"] %>
    <dl class="hx-grid hx-gap-2 grid-cols-2 mt-1 pl-1">
      <%= generate_members(:definition_field) %>
    </dl>
    <% end %>
    <% end %>
  EOF

  @@definition_field_template = ERB.new <<~'EOF'
    <dt class="col-span-1 mt-0 mb-0"><a href="#<%= ref %>"><%= name %></a></dd>
    <dd class="col-span-1 mt-0 mb-0 p-inline-start-none"><% if overload_index %><em class="hx-text-sm">(overload <%= overload_index %>)</em> <% end %><%= md(summary) %></dd>
  EOF

  @@field_template = ERB.new <<~'EOF'
    <dt><a href="#<%= ref %>"><%= name %></a> <em class="hx-text-sm"><%= md(summary) %></em></dt>
    <dd><% if literal %><code><%= literal %></code> <% elsif typ %><code><%= typ %></code> <% end %><%= md(short_description) %></dd>
  EOF

  @@templates = {
    enum: @@enum_template,
    dunno: @@dunno_template,
    field: @@field_template,
    table: @@table_template,
    definition: @@definition_template,
    definition_field: @@definition_field_template,
    class: @@class_template,
    fn: @@fn_template,
    alias: @@alias_template,
  }
end
