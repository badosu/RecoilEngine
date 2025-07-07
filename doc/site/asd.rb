require 'ostruct'
require 'erb'
require 'json'
require 'set'

def md(string)
  string ||= ""
  string = string.gsub('"', '\"')
  "{{< md \"#{string}\" >}}"
end

def h(level, name, ref, id = nil)
  level = level + 2

  if id
    id = " id=\"#{ref}-h\""
  end

  %Q{<h#{level}#{id}>#{name}<span class="hx-absolute -hx-mt-20" id="#{ref}"></span><a href="##{ref}" class="subheading-anchor" aria-label="Permalink for this section">#{yield if block_given?}</a></h#{level}>}
end

class Member < OpenStruct
  @@other_matcher = Regexp.compile('\@\*(\w+)\*( (.*))?')

  enum_template = ERB.new <<~'EOF'
    <%= h(level, full_name, ref) %>

    <% if description %>

    <%= description %>
    <% end %>
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

  global_template = ERB.new <<~'EOF'
    <%= h(level, full_name, ref) %>
    <% if description %>

    <%= description %>
    <% end %>
    <% if deprecated %>

    Deprecated <%= deprecated %>
    <% end %>
    <% if see %>

    See <%= see %>
    <% end %>
    <%= generate_members() %>
  EOF

  fn_template = ERB.new <<~'EOF'
    <%= h(level, full_name, ref) %>

    <div class="pl-1">
    <% if description %>

    <%= description %>
    <% end %>
    <% if deprecated %>

    Deprecated <%= deprecated %>
    <% end %>
    <% if see %>

    See <%= see %>
    <% end %>

    <%= h(level + 1, "Arguments", "#{full_name}_arguments") %>

    <% if params&.empty? %>
      No arguments
    <% else %>
    <dl class="pl-1">
    <% params.each do |param| %>
      <dt><%= param["name"] %></dt>
      <dd>
        <code><%= param["typ"] %></code> <%= param["desc"] %>
      </dd>
    <% end %>
    </dl>
    <% end %>

    <%= h(level + 1, "Returns", "#{full_name}_returns") %>

    <% if returns&.empty? %>
      No return value (`nil`)
    <% else %>
    <dl class="pl-1">
    <% returns.each do |return_value| %>
      <dt><%= return_value["name"] %></dt>
      <dd>
        <code><%= return_value["typ"] %></code> <%= return_value["desc"] %>
      </dd>
    <% end %>
    </dl>
    <% end %>
    </div>
  EOF

  dunno_template = ERB.new <<~'EOF'
    <%= h(level, full_name, full_name) %>
    <% if type == :alias %>
      <em>(alias)</em>
    <% end %>
    <% if description %>

    <%= description %>
    <% end %>
    <% if deprecated %>

    Deprecated <%= deprecated %>
    <% end %>
    <% if see %>

    See <%= see %>
    <% end %>
    <%= generate_members() %>
  EOF

  definition_template = ERB.new <<~'EOF'
    <% if type == :alias || custom["helper"] %>
    <% elsif custom["no_toc_contents"] || type == :enum %>
    <div class="hx-grid hx-gap-2 grid-cols-2 mt-1 align-baseline">
      <h3 id="<%= full_name %>-heading"><a href="#<%= ref %>"><%= name %></a></h3>
      <p><%= summary %></p>
    </div>
    <% else %>
    <h3 id="<%= full_name %>-heading"><a href="#<%= ref %>"><%= name %></a></h3>
    <p><%= summary %></p>

    <dl class="hx-grid hx-gap-2 grid-cols-2 mt-1">
      <%= generate_members(:definition_field) %>
    </dl>
    <% end %>
  EOF

  definition_field_template = ERB.new <<~'EOF'
    <dt class="col-span-1 m-0"><a href="#<%= ref %>"><%= name %></a></dd>
    <dd class="col-span-1 m-0 p-inline-start-none"><%= summary %></dd>
  EOF

  field_template = ERB.new <<~'EOF'
    <dt><a href="#<%= ref %>"><%= name %></a></dt>
    <dd><% if self.typ %><code><%= self.typ %></code><% end %> <%= md(self.summary) %></dd>
  EOF

  @@templates = {
    enum: enum_template,
    global: global_template,
    dunno: dunno_template,
    field: field_template,
    definition: definition_template,
    definition_field: definition_field_template,
    fn: fn_template
  }

  @@known_templates = Set.new(@@templates.keys)

  @@top_level = Set.new(["Spring", "Callins", "UnsyncedCallins", "gl", "GL", "RmlUi"])

  def initialize(attributes, parent = nil)
    super(attributes)

    initialize_attributes(parent)
  end

  def generate_members(member_type = nil, opts = {})
    if member_type
      self.children.map {|c| c.generate(member_type) }.join("\n")
    else
      self.children.map(&:generate).join("\n")
    end
  end

  def generate(internal_type = nil, opts = {})
    internal_type = internal_type || self.type

    template = @@templates.include?(internal_type) ? internal_type : :dunno

    @@templates[template].result(binding)
  end

  private

  def initialize_attributes(parent)
    self.level = parent ? parent.level + 1 : 0

    if parent
      self.full_name = "#{parent.full_name}.#{self.name}"
    else
      self.full_name = self.name || "noname?"
    end

    self.ref = full_name
    self.type = self.type.to_sym

    members = self.members || []

    self.children = members.map {|m| Member.new(m, self)}

    self.children = self.children.sort do |m1, m2|
      helper1 = m1.custom["helper"]
      helper2 = m2.custom["helper"]

      # Tagged helpers should be at the utmost bottom
      if helper1 && helper1 == helper2
        next 0
      elsif helper1
        next 1
      elsif helper2
        next -1
      elsif m1.type == m2.type
        next m1.full_name <=> m2.full_name
      # Alias next to bottom
      elsif (m1.type == :alias)
        next 1
      elsif m1.type == :table
        next -1
      end

      m1.type <=> m2.type
    end

    if self.description
      self.summary = self.description.split("\n").first()
    end

    extract_custom_tags()
  end

  def extract_custom_tags()
    other = self.other

    if !other
      self.custom = {}
      self.section = nil

      return
    end

    custom = other.split("\n\n").reduce({}) do |acc, el|
      name, _, value = el.match(@@other_matcher).captures

      if name
        acc[name] = value || true
      end

      acc
    end

    self.custom = custom
    self.section = custom["section"]
  end
end

class Generator
  def initialize(data_file)
    data = JSON.load_file(data_file)

    @globals = (data["globals"] + data["types"]).reduce({}) do |acc, el|
      member = Member.new(el)

      acc[member.name] = member

      acc
    end
  end

  def generate
    top_level = ["Spring", "Callins", "UnsyncedCallins", "gl", "GL", "RmlUi"]
      .each_with_index
      .reduce({}) {|acc, (el, i)| acc[el] = i + 1; acc }

    members = @globals.sort do |g1, g2|
      m1 = g1[1]
      m2 = g2[1]

      helper1 = m1.custom["helper"]
      helper2 = m2.custom["helper"]

      # if m1.name.include?("Spring") || m2.name.include?("Spring")
      #   puts m1.full_name
      #   puts top_level[m1.full_name]
      #   if helper1
      #   puts helper1
      #   end
      #   puts m2.full_name
      #   puts top_level[m2.full_name]
      #   if helper2
      #   puts helper2
      #   end
      #   puts "----"
      # end

      # Tagged helpers should be at the utmost bottom
      if helper1 && (helper1 == helper2)
        next 0
      elsif helper1
        next 1
      elsif helper2
        next -1
      elsif m1.type == m2.type
        include1 = top_level[m1.full_name]
        include2 = top_level[m2.full_name]

        if include1 == include2
          next include1 <=> include2
        elsif include1
          next -1
        else
          next 1
        end
      elsif m1.type == :alias
        next 1
      elsif m1.type == :table
        next -1
      end

      m1.type <=> m2.type
    end

    # members = @globals.select { |k, _v| top_level.include?(k) }

    inner_content = members.map { |name, el| el.generate() }.join("\n")

    <<~EOF
      +++
      title = "Lua API"
      +++

      ## Table of Contents

      #{members.map { |name, el| el.generate(:definition) }.join("\n")}

      #{inner_content}
    EOF
  end
end

def usage_error()
  puts <<~'EOF'
    Usage:

    --doc: An emmylua-doc-cli compatible documentation file
    --out: A file path to be written

    Example:

    --doc=doc.json --out=doc.md
  EOF

  exit(1)
end

def run()
  args = Hash[ARGV.join(' ').scan(/--?([^=\s]+)(?:=(\S+))?/)]
  args = OpenStruct.new(args)

  if not args.doc or not File.exist?(args.doc)
    puts "Error: Invalid or non existent documentation file #{args.doc}\n\n"

    usage_error()
  end

  if not args.out
    puts "Error: Invalid or non existent out path #{args.out}\n\n"

    usage_error()
  end

  generator = Generator.new(args.doc)

  generated = generator.generate()

  File.open(args.out, "w") { |f| f.write(generated) }
end

run()
