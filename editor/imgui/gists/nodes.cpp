// Prototype of standalone node graph editor for ImGui
// Thread: https://github.com/ocornut/imgui/issues/306
//
// This is based on code by:
// @emoon https://gist.github.com/emoon/b8ff4b4ce4f1b43e79f2
// @ocornut https://gist.github.com/ocornut/7e9b3ec566a333d725d4
// @flix01 https://github.com/Flix01/imgui/blob/b248df2df98af13d4b7dbb70c92430afc47a038a/addons/imguinodegrapheditor/imguinodegrapheditor.cpp#L432

#include "Nodes.h"

namespace ImGui
{
	Nodes::Nodes()
	{
		id_ = 0;
		element_.Reset();
		canvas_scale_ = 1.0f;
	}

	Nodes::~Nodes()
	{
	}

	Nodes::Node* Nodes::GetHoverNode(ImVec2 offset, ImVec2 pos)
	{
		for (auto& node : nodes_)
		{
			ImRect rect((node->position_ * canvas_scale_) + offset, ((node->position_ + node->size_) * canvas_scale_) + offset);

			rect.Expand(2.0f);
			
			if (rect.Contains(pos))
			{
				return node->Get();
			}
		}

		return nullptr;
	}

	void Nodes::RenderLines(ImDrawList* draw_list, ImVec2 offset)
	{
		for (auto& node : nodes_)
		{
			for (auto& connection : node->inputs_)
			{
				if (connection->connections_ == 0)
				{
					continue;
				}
	
				ImVec2 p1 = offset;
				ImVec2 p4 = offset;

				if (connection->target_->state_ > 0)
				{	// we are connected to output of not a collapsed node
					p1 += ((connection->target_->position_ + connection->input_->position_) * canvas_scale_);
				}
				else
				{	// we are connected to output of a collapsed node
					p1 += ((connection->target_->position_ + ImVec2(connection->target_->size_.x, connection->target_->size_.y / 2.0f)) * canvas_scale_);
				}
				
				if (node->state_ > 0)
				{	// we are not a collapsed node
					p4 += ((node->position_ + connection->position_) * canvas_scale_);
				}
				else
				{	// we are a collapsed node
					p4 += ((node->position_ + ImVec2(0.0f, node->size_.y / 2.0f)) * canvas_scale_);
				}

				// default bezier control points
				ImVec2 p2 = p1 + (ImVec2(+50.0f, 0.0f) * canvas_scale_);
				ImVec2 p3 = p4 + (ImVec2(-50.0f, 0.0f) * canvas_scale_);

				if (element_.state_ == NodesState_Default)
				{
					const float distance_squared = GetSquaredDistanceToBezierCurve(ImGui::GetIO().MousePos, p1, p2, p3, p4);

					if (distance_squared < (10.0f * 10.0f))
					{
						element_.Reset(NodesState_HoverConnection);

						element_.rect_ = ImRect
						(
							(connection->target_->position_ + connection->input_->position_),
							(node->position_ + connection->position_)
						);

						element_.node_ = node->Get();
						element_.connection_ = connection.get();
					}
				}
	
				bool selected = false;
				selected |= element_.state_ == NodesState_SelectedConnection;
				selected |= element_.state_ == NodesState_DragingConnection;
				selected &= element_.connection_ == connection.get();
							
				draw_list->AddBezierCurve(p1, p2, p3, p4, ImColor(0.5f, 0.5f, 0.5f, 1.0f), 2.0f * canvas_scale_);

				if (selected)
				{
					draw_list->AddBezierCurve(p1, p2, p3, p4, ImColor(1.0f, 1.0f, 1.0f, 0.25f), 4.0f * canvas_scale_);
				}
			}
		}
	}

	void Nodes::DisplayNodes(ImDrawList* drawList, ImVec2 offset)
	{
		ImGui::SetWindowFontScale(canvas_scale_);

		for (auto& node : nodes_)
		{
			DisplayNode(drawList, offset, *node);
		}			

		ImGui::SetWindowFontScale(1.0f);
	}

	Nodes::Node* Nodes::CreateNodeFromType(ImVec2 pos, const NodeType& type)
	{
		auto node = std::make_unique<Node>();

		////////////////////////////////////////////////////////////////////////////////
		
		node->id_ = -++id_;
		node->name_ = type.name_ + std::to_string(id_).c_str();
		node->position_ = pos;

		{
			auto &inputs = node->inputs_;
			std::for_each
			(
				type.inputs_.begin(),
				type.inputs_.end(),
				[&inputs](auto& element)
				{
					auto connection = std::make_unique<Connection>();
					connection->name_ = element.first;
					connection->type_ = element.second;

					inputs.push_back(std::move(connection));
				}
			);

			auto &outputs = node->outputs_;
			std::for_each
			(
				type.outputs_.begin(),
				type.outputs_.end(),
				[&outputs](auto& element)
				{
					auto connection = std::make_unique<Connection>();
					connection->name_ = element.first;
					connection->type_ = element.second;
			
					outputs.push_back(std::move(connection));
				}
			);
		}

		////////////////////////////////////////////////////////////////////////////////

		ImVec2 title_size = ImGui::CalcTextSize(node->name_.c_str());

		const float vertical_padding = 1.5f;

		////////////////////////////////////////////////////////////////////////////////

		ImVec2 inputs_size;
		for (auto& connection : node->inputs_)
		{
			ImVec2 name_size = ImGui::CalcTextSize(connection->name_.c_str());
			inputs_size.x = ImMax(inputs_size.x, name_size.x);
			inputs_size.y += name_size.y * vertical_padding;
		}
		
		ImVec2 outputs_size;
		for (auto& connection : node->outputs_)
		{
			ImVec2 name_size = ImGui::CalcTextSize(connection->name_.c_str());
			outputs_size.x = ImMax(outputs_size.x, name_size.x);
			outputs_size.y += name_size.y * vertical_padding;
		}

		////////////////////////////////////////////////////////////////////////////////

		node->size_.x = ImMax((inputs_size.x + outputs_size.x), title_size.x);
		node->size_.x += title_size.y * 6.0f;

		node->collapsed_height = (title_size.y * 2.0f);
		node->full_height = (title_size.y * 3.0f) + ImMax(inputs_size.y, outputs_size.y);

		node->size_.y = node->full_height;
		
		node->position_ -= node->size_ / 2.0f;
		
		////////////////////////////////////////////////////////////////////////////////

		inputs_size = ImVec2(title_size.y * 0.75f, title_size.y * 2.5f);
		for (auto& connection : node->inputs_)
		{
			const float half = ((ImGui::CalcTextSize(connection->name_.c_str()).y * vertical_padding) / 2.0f);

			inputs_size.y += half;
			connection->position_ = ImVec2(inputs_size.x, inputs_size.y);
			inputs_size.y += half;
		}

		outputs_size = ImVec2(node->size_.x - (title_size.y * 0.75f), title_size.y * 2.5f);
		for (auto& connection : node->outputs_)
		{
			const float half = ((ImGui::CalcTextSize(connection->name_.c_str()).y * vertical_padding) / 2.0f);

			outputs_size.y += half;
			connection->position_ = ImVec2(outputs_size.x, outputs_size.y);
			outputs_size.y += half;
		}
	
		////////////////////////////////////////////////////////////////////////////////

		nodes_.push_back(std::move(node));
		return nodes_.back().get();
	}

	void Nodes::UpdateScroll()
	{
		////////////////////////////////////////////////////////////////////////////////

		{
			ImVec2 scroll;

			if (ImGui::GetIO().KeyShift && !ImGui::GetIO().KeyCtrl && !ImGui::IsMouseDown(0) && !ImGui::IsMouseDown(1) && !ImGui::IsMouseDown(2))
			{
				scroll.x = ImGui::GetIO().MouseWheel * 24.0f;
			}

			if (!ImGui::GetIO().KeyShift && !ImGui::GetIO().KeyCtrl && !ImGui::IsMouseDown(0) && !ImGui::IsMouseDown(1) && !ImGui::IsMouseDown(2))
			{
				scroll.y = ImGui::GetIO().MouseWheel * 24.0f;
			}

			if (ImGui::IsMouseDragging(1, 6.0f) && !ImGui::IsMouseDown(0) && !ImGui::IsMouseDown(2))
			{
				scroll += ImGui::GetIO().MouseDelta;
			}

			canvas_scroll_ += scroll;

		}

		////////////////////////////////////////////////////////////////////////////////

		{
			ImVec2 mouse = canvas_mouse_;

			float zoom = 0.0f;

			if (!ImGui::GetIO().KeyShift && !ImGui::IsMouseDown(0) && !ImGui::IsMouseDown(1))
			{
				if (ImGui::GetIO().KeyCtrl)
				{
					zoom += ImGui::GetIO().MouseWheel;
				}

				if (ImGui::IsMouseDragging(2, 6.0f))
				{
					zoom -= ImGui::GetIO().MouseDelta.y;
					mouse -= ImGui::GetMouseDragDelta(2, 6.0f);
				}
			}

			ImVec2 focus = (mouse - canvas_scroll_) / canvas_scale_;

			if (zoom < 0.0f)
			{
				canvas_scale_ /= 1.05f;
			}

			if (zoom > 0.0f)
			{
				canvas_scale_ *= 1.05f;
			}

			if (canvas_scale_ < 0.3f)
			{
				canvas_scale_ = 0.3f;
			}

			if (canvas_scale_ > 3.0f)
			{
				canvas_scale_ = 3.0f;
			}

			focus = canvas_scroll_ + (focus * canvas_scale_);
			canvas_scroll_ += (mouse - focus);
		}

		////////////////////////////////////////////////////////////////////////////////
	}

	void Nodes::UpdateState(ImVec2 offset)
	{
		if (element_.state_ == NodesState_HoverNode && ImGui::IsMouseDoubleClicked(0))
		{		
			if (element_.node_->state_ < 0)
			{	// collapsed node goes to full
				element_.node_->size_.y = element_.node_->full_height;
			}
			else
			{	// full node goes to collapsed
				element_.node_->size_.y = element_.node_->collapsed_height;
			}

			element_.node_->state_ = -element_.node_->state_;
		}

		switch (element_.state_)
		{		
			case NodesState_Default:
			{
				if (ImGui::IsMouseDown(0) && !ImGui::IsMouseDown(1) && !ImGui::IsMouseDown(2))
				{
					ImRect canvas = ImRect(ImVec2(0.0f, 0.0f), canvas_size_);

					if (!canvas.Contains(canvas_mouse_))
					{
						break;
					}

					element_.Reset(NodesState_SelectingEmpty);

					element_.position_ = ImGui::GetIO().MousePos;
					element_.rect_.Min = ImGui::GetIO().MousePos;
					element_.rect_.Max = ImGui::GetIO().MousePos;
				}
			} break;

			// helper: just block all states till next update
			case NodesState_Block:
			{
				element_.Reset(); 
			} break;

			case NodesState_HoverConnection:
			{
				const float distance_squared = GetSquaredDistanceToBezierCurve
				(
					ImGui::GetIO().MousePos,
					offset + (element_.rect_.Min * canvas_scale_),
					offset + (element_.rect_.Min * canvas_scale_) + (ImVec2(+50.0f, 0.0f) * canvas_scale_),
					offset + (element_.rect_.Max * canvas_scale_) + (ImVec2(-50.0f, 0.0f) * canvas_scale_),
					offset + (element_.rect_.Max * canvas_scale_)
				);
				
				if (distance_squared > (10.0f * 10.0f))
				{
					element_.Reset();
					break;
				}

				if (ImGui::IsMouseDown(0))
				{
					element_.state_ = NodesState_SelectedConnection;
				}

			} break;

			case NodesState_DragingInput:
			{
				if (!ImGui::IsMouseDown(0) || ImGui::IsMouseClicked(1))
				{
					element_.Reset(NodesState_Block);
					break;
				}

				ImVec2 p1 = offset + (element_.position_ * canvas_scale_);
				ImVec2 p2 = p1 + (ImVec2(-50.0f, 0.0f) * canvas_scale_);
				ImVec2 p3 = ImGui::GetIO().MousePos + (ImVec2(+50.0f, 0.0f) * canvas_scale_);
				ImVec2 p4 = ImGui::GetIO().MousePos;

				ImGui::GetWindowDrawList()->AddBezierCurve(p1, p2, p3, p4, ImColor(0.0f, 1.0f, 0.0f, 1.0f), 2.0f * canvas_scale_);
			} break;

			case NodesState_DragingInputValid:
			{
				element_.state_ = NodesState_DragingInput;

				if (ImGui::IsMouseClicked(1))
				{
					element_.Reset(NodesState_Block);
					break;
				}

				ImVec2 p1 = offset + (element_.position_ * canvas_scale_);
				ImVec2 p2 = p1 + (ImVec2(-50.0f, 0.0f) * canvas_scale_);
				ImVec2 p3 = ImGui::GetIO().MousePos + (ImVec2(+50.0f, 0.0f) * canvas_scale_);
				ImVec2 p4 = ImGui::GetIO().MousePos;

				ImGui::GetWindowDrawList()->AddBezierCurve(p1, p2, p3, p4, ImColor(0.0f, 1.0f, 0.0f, 1.0f), 2.0f * canvas_scale_);
			} break;

			case NodesState_DragingOutput:
			{
				if (!ImGui::IsMouseDown(0) || ImGui::IsMouseClicked(1))
				{
					element_.Reset(NodesState_Block);
					break;
				}

				ImVec2 p1 = offset + (element_.position_ * canvas_scale_);
				ImVec2 p2 = p1 + (ImVec2(+50.0f, 0.0f) * canvas_scale_);
				ImVec2 p3 = ImGui::GetIO().MousePos + (ImVec2(-50.0f, 0.0f) * canvas_scale_);
				ImVec2 p4 = ImGui::GetIO().MousePos;

				ImGui::GetWindowDrawList()->AddBezierCurve(p1, p2, p3, p4, ImColor(0.0f, 1.0f, 0.0f, 1.0f), 2.0f * canvas_scale_);
			} break;

			case NodesState_DragingOutputValid:
			{
				element_.state_ = NodesState_DragingOutput;

				if (ImGui::IsMouseClicked(1))
				{
					element_.Reset(NodesState_Block);
					break;
				}

				ImVec2 p1 = offset + (element_.position_ * canvas_scale_);
				ImVec2 p2 = p1 + (ImVec2(+50.0f, 0.0f) * canvas_scale_);
				ImVec2 p3 = ImGui::GetIO().MousePos + (ImVec2(-50.0f, 0.0f) * canvas_scale_);
				ImVec2 p4 = ImGui::GetIO().MousePos;

				ImGui::GetWindowDrawList()->AddBezierCurve(p1, p2, p3, p4, ImColor(0.0f, 1.0f, 0.0f, 1.0f), 2.0f * canvas_scale_);
			} break;

			case NodesState_SelectingEmpty:
			{
				if (!ImGui::IsMouseDown(0))
				{
					element_.Reset(NodesState_Block);
					break;
				}

				element_.rect_.Min = ImMin(element_.position_, ImGui::GetIO().MousePos);
				element_.rect_.Max = ImMax(element_.position_, ImGui::GetIO().MousePos);
			} break;

			case NodesState_SelectingValid:
			{
				if (!ImGui::IsMouseDown(0))
				{
					element_.Reset(NodesState_Selected);
					break;
				}

				element_.rect_.Min = ImMin(element_.position_, ImGui::GetIO().MousePos);
				element_.rect_.Max = ImMax(element_.position_, ImGui::GetIO().MousePos);

				element_.state_ = NodesState_SelectingEmpty;
			} break;

			case NodesState_SelectingMore:
			{
				element_.rect_.Min = ImMin(element_.position_, ImGui::GetIO().MousePos);
				element_.rect_.Max = ImMax(element_.position_, ImGui::GetIO().MousePos);

				if (ImGui::IsMouseDown(0) && ImGui::GetIO().KeyShift)
				{
					break;
				}

				for (auto& node : nodes_)
				{
					ImVec2 node_rect_min = offset + (node->position_ * canvas_scale_);
					ImVec2 node_rect_max = node_rect_min + (node->size_ * canvas_scale_);

					ImRect node_rect(node_rect_min, node_rect_max);

					if (ImGui::GetIO().KeyCtrl && element_.rect_.Overlaps(node_rect))
					{
						node->id_ = -abs(node->id_); // add "selected" flag
						continue;
					}
					
					if (!ImGui::GetIO().KeyCtrl && element_.rect_.Contains(node_rect))
					{
						node->id_ = -abs(node->id_); // add "selected" flag
						continue;
					}
				}

				element_.Reset(NodesState_Selected);
			} break;

			case NodesState_Selected:
			{
				// delete all selected nodes
				if (ImGui::IsKeyPressed(ImGui::GetIO().KeyMap[ImGuiKey_Delete]))
				{
					std::vector<std::unique_ptr<Node>> replacement;
					replacement.reserve(nodes_.size());

					// delete connections
					for (auto& node : nodes_)
					{	
						for (auto& connection : node->inputs_)
						{
							if (connection->connections_ == 0)
							{
								continue;
							}

							if (node->id_ < 0)
							{
								connection->input_->connections_--;
							}
							
							if (connection->target_->id_ <0)
							{
								connection->target_ = nullptr;
								connection->input_ = nullptr;
								connection->connections_ = 0;
							}
						}
					}

					// save not selected nodes
					for (auto& node : nodes_)
					{
						if (node->id_ > 0)
						{
							replacement.push_back(std::move(node));
						}
					}
					
					nodes_ = std::move(replacement);
					
					element_.Reset();
					break;
				}

				// no action taken
				if (!ImGui::IsMouseClicked(0))
				{
					break;
				}

				element_.Reset();
				auto hovered = GetHoverNode(offset, ImGui::GetIO().MousePos);

				// empty area under the mouse
				if (!hovered)
				{
					element_.position_ = ImGui::GetIO().MousePos;
					element_.rect_.Min = ImGui::GetIO().MousePos;
					element_.rect_.Max = ImGui::GetIO().MousePos;

					if (ImGui::GetIO().KeyShift)
					{
						element_.state_ = NodesState_SelectingMore;
					}
					else
					{
						element_.state_ = NodesState_SelectingEmpty;
					}
					break;
				}
				
				// lets select node under the mouse
				if (ImGui::GetIO().KeyShift)
				{
					hovered->id_ = -abs(hovered->id_);
					element_.state_ = NodesState_DragingSelected;
					break;
				}

				// lets toggle selection of a node under the mouse
				if (!ImGui::GetIO().KeyShift && ImGui::GetIO().KeyCtrl)
				{
					if (hovered->id_ > 0)
					{
						hovered->id_ = -abs(hovered->id_);
						element_.state_ = NodesState_DragingSelected;
					}
					else
					{
						hovered->id_ = abs(hovered->id_);
						element_.state_ = NodesState_Selected;
					}
					break;
				}

				// lets start dragging
				if (hovered->id_ < 0)
				{
					element_.state_ = NodesState_DragingSelected;
					break;
				}
				
				// not selected node clicked, lets jump selection to it
				for (auto& node : nodes_)
				{
					if (node.get() != hovered)
					{
						node->id_ = abs(node->id_);
					}
				}
			} break;

			case NodesState_DragingSelected:
			{
				if (!ImGui::IsMouseDown(0))
				{
					if (element_.node_)
					{
						if (ImGui::GetIO().KeyShift || ImGui::GetIO().KeyCtrl)
						{
							element_.Reset(NodesState_Selected);
							break;
						}

						element_.state_ = NodesState_HoverNode;
					}
					else
					{
						element_.Reset(NodesState_Selected);
					}
				}
				else
				{
					for (auto& node : nodes_)
					{
						if (node->id_ < 0)
						{
							node->position_ += ImGui::GetIO().MouseDelta / canvas_scale_;
						}
					}
				}
			} break;

			case NodesState_SelectedConnection:
			{
				if (ImGui::IsMouseClicked(1))
				{
					element_.Reset(NodesState_Block);
					break;
				}

				if (ImGui::IsMouseDown(0))
				{
					const float distance_squared = GetSquaredDistanceToBezierCurve
					(
						ImGui::GetIO().MousePos,
						offset + (element_.rect_.Min * canvas_scale_),
						offset + (element_.rect_.Min * canvas_scale_) + (ImVec2(+50.0f, 0.0f) * canvas_scale_),
						offset + (element_.rect_.Max * canvas_scale_) + (ImVec2(-50.0f, 0.0f) * canvas_scale_),
						offset + (element_.rect_.Max * canvas_scale_)
					);

					if (distance_squared > (10.0f * 10.0f))
					{
						element_.Reset();
						break;
					}

					element_.state_ = NodesState_DragingConnection;
				}
			} break;

			case NodesState_DragingConnection:
			{
				if (!ImGui::IsMouseDown(0))
				{
					element_.state_ = NodesState_SelectedConnection;
					break;
				}

				if (ImGui::IsMouseClicked(1))
				{
					element_.Reset(NodesState_Block);
					break;
				}

				element_.node_->position_ += ImGui::GetIO().MouseDelta / canvas_scale_;
				element_.connection_->target_->position_ += ImGui::GetIO().MouseDelta / canvas_scale_;
			} break;
		}
	}

	void Nodes::DisplayNode(ImDrawList* drawList, ImVec2 offset, Node& node)
	{
		ImGui::PushID(abs(node.id_));
		ImGui::BeginGroup();

		ImVec2 node_rect_min = offset + (node.position_ * canvas_scale_);
		ImVec2 node_rect_max = node_rect_min + (node.size_ * canvas_scale_);

		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("Node", node.size_ * canvas_scale_);
		
		////////////////////////////////////////////////////////////////////////////////

		// state machine for node hover/drag
		{
			bool node_hovered = ImGui::IsItemHovered();
			bool node_active = ImGui::IsItemActive();

			if (node_hovered && element_.state_ == NodesState_HoverNode)
			{
				element_.node_ = node.Get();

				if (node_active)
				{
					node.id_ = -abs(node.id_); // add "selected" flag
					element_.state_ = NodesState_DragingSelected;
				}
			}

			if (node_hovered && element_.state_ == NodesState_Default)
			{
				element_.node_ = node.Get();

				if (node_active)
				{
					node.id_ = -abs(node.id_); // add "selected" flag
					element_.state_ = NodesState_DragingSelected;
				}
				else
				{
					element_.state_ = NodesState_HoverNode;
				}
			}

			if (!node_hovered && element_.state_ == NodesState_HoverNode)
			{
				if (element_.node_ == node.Get())
				{
					element_.Reset();
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		bool consider_hover = element_.node_ ? element_.node_ == node.Get() : false;

		////////////////////////////////////////////////////////////////////////////////

		if (element_.state_ != NodesState_Selected && element_.state_ != NodesState_DragingSelected && element_.state_ != NodesState_SelectingMore)
		{
			node.id_ = abs(node.id_); // remove "selected" flag
		}

		////////////////////////////////////////////////////////////////////////////////

		bool consider_select = false;
		consider_select |= element_.state_ == NodesState_SelectingEmpty;
		consider_select |= element_.state_ == NodesState_SelectingValid;
		consider_select |= element_.state_ == NodesState_SelectingMore;

		if (consider_select)
		{		
			bool select_it = false;
		
			ImRect node_rect(node_rect_min,	node_rect_max);
		
			if (ImGui::GetIO().KeyCtrl)
			{
				select_it |= element_.rect_.Overlaps(node_rect);
			}
			else
			{
				select_it |= element_.rect_.Contains(node_rect);
			}
		
			consider_hover |= select_it;

			if (select_it && element_.state_ != NodesState_SelectingMore)
			{
				node.id_ = -abs(node.id_); // add "selected" flag
				element_.state_ = NodesState_SelectingValid;
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		ImVec2 title_name_size = ImGui::CalcTextSize(node.name_.c_str());
		const float corner = title_name_size.y / 2.0f;

		{		
			ImVec2 title_area;
			title_area.x = node_rect_max.x;
			title_area.y = node_rect_min.y + (title_name_size.y * 2.0f);

			ImVec2 title_pos;
			title_pos.x = node_rect_min.x + ((title_area.x - node_rect_min.x) / 2.0f) - (title_name_size.x / 2.0f);
			
			if (node.state_ > 0)
			{
				drawList->AddRectFilled(node_rect_min, node_rect_max, ImColor(0.25f, 0.25f, 0.25f, 0.9f), corner, ImDrawCornerFlags_All);
				drawList->AddRectFilled(node_rect_min, title_area, ImColor(0.25f, 0.0f, 0.125f, 0.9f), corner, ImDrawCornerFlags_Top);

				title_pos.y = node_rect_min.y + ((title_name_size.y * 2.0f) / 2.0f) - (title_name_size.y / 2.0f);
			}
			else
			{
				drawList->AddRectFilled(node_rect_min, node_rect_max, ImColor(0.25f, 0.0f, 0.125f, 0.9f), corner, ImDrawCornerFlags_All);

				title_pos.y = node_rect_min.y + ((node_rect_max.y - node_rect_min.y) / 2.0f) - (title_name_size.y / 2.0f);
			}

			ImGui::SetCursorScreenPos(title_pos);
			ImGui::Text("%s", node.name_.c_str());
		}

		////////////////////////////////////////////////////////////////////////////////
		
		if (node.state_ > 0)
		{
			////////////////////////////////////////////////////////////////////////////////

			for (auto& connection : node.inputs_)
			{
				if (connection->type_ == ConnectionType_None)
				{
					continue;
				}

				bool consider_io = false;

				ImVec2 input_name_size = ImGui::CalcTextSize(connection->name_.c_str());
				ImVec2 connection_pos = node_rect_min + (connection->position_ * canvas_scale_);

				{
					ImVec2 pos = connection_pos;
					pos += ImVec2(input_name_size.y * 0.75f, -input_name_size.y / 2.0f);

					ImGui::SetCursorScreenPos(pos);
					ImGui::Text("%s", connection->name_.c_str());
				}

				if (IsConnectorHovered(connection_pos, (input_name_size.y / 2.0f)))
				{
					consider_io |= element_.state_ == NodesState_Default;
					consider_io |= element_.state_ == NodesState_HoverConnection;
					consider_io |= element_.state_ == NodesState_HoverNode;

					// from nothing to hovered input
					if (consider_io)
					{
						element_.Reset(NodesState_HoverIO);
						element_.node_ = node.Get();
						element_.connection_ = connection.get();
						element_.position_ = node.position_ + connection->position_;
					}

					// we could start draging input now
					if (ImGui::IsMouseClicked(0) && element_.connection_ == connection.get())
					{
						element_.state_ = NodesState_DragingInput;

						// remove connection from this input
						if (connection->input_)
						{
							connection->input_->connections_--;
						}

						connection->target_ = nullptr;
						connection->input_ = nullptr;
						connection->connections_ = 0;
					}

					consider_io = true;
				}
				else if (element_.state_ == NodesState_HoverIO && element_.connection_ == connection.get())
				{
					element_.Reset(); // we are not hovering this last hovered input anymore
				}

				////////////////////////////////////////////////////////////////////////////////

				ImColor color = ImColor(0.5f, 0.5f, 0.5f, 1.0f);

				if (connection->connections_ > 0)
				{
					drawList->AddCircleFilled(connection_pos, (input_name_size.y / 3.0f), color);
				}

				// currently we are dragin some output, check if there is a possibilty to connect here (this input)
				if (element_.state_ == NodesState_DragingOutput || element_.state_ == NodesState_DragingOutputValid)
				{
					// check is draging output are not from the same node
					if (element_.node_ != node.Get() && element_.connection_->type_ == connection->type_)
					{
						color = ImColor(0.0f, 1.0f, 0.0f, 1.0f);

						if (consider_io)
						{
							element_.state_ = NodesState_DragingOutputValid;
							drawList->AddCircleFilled(connection_pos, (input_name_size.y / 3.0f), color);

							if (!ImGui::IsMouseDown(0))
							{
								if (connection->input_)
								{
									connection->input_->connections_--;
								}

								connection->target_ = element_.node_;
								connection->input_ = element_.connection_;
								connection->connections_ = 1;
								element_.connection_->connections_++;

								element_.Reset(NodesState_HoverIO);
								element_.node_ = node.Get();
								element_.connection_ = connection.get();
								element_.position_ = node_rect_min + connection->position_;
							}
						}
					}
				}

				consider_io |= element_.state_ == NodesState_HoverIO;
				consider_io |= element_.state_ == NodesState_DragingInput;
				consider_io |= element_.state_ == NodesState_DragingInputValid;
				consider_io &= element_.connection_ == connection.get();

				if (consider_io)
				{
					color = ImColor(0.0f, 1.0f, 0.0f, 1.0f);

					if (element_.state_ != NodesState_HoverIO)
					{
						drawList->AddCircleFilled(connection_pos, (input_name_size.y / 3.0f), color);
					}
				}

				drawList->AddCircle(connection_pos, (input_name_size.y / 3.0f), color, ((int)(6.0f * canvas_scale_) + 10), (1.5f * canvas_scale_));
			}

			////////////////////////////////////////////////////////////////////////////////

			for (auto& connection : node.outputs_)
			{
				if (connection->type_ == ConnectionType_None)
				{
					continue;
				}

				bool consider_io = false;

				ImVec2 output_name_size = ImGui::CalcTextSize(connection->name_.c_str());
				ImVec2 connection_pos = node_rect_min + (connection->position_ * canvas_scale_);

				{
					ImVec2 pos = connection_pos;
					pos += ImVec2(-output_name_size.x - (output_name_size.y * 0.75f), -output_name_size.y / 2.0f);

					ImGui::SetCursorScreenPos(pos);
					ImGui::Text("%s", connection->name_.c_str());
				}

				if (IsConnectorHovered(connection_pos, (output_name_size.y / 2.0f)))
				{
					consider_io |= element_.state_ == NodesState_Default;
					consider_io |= element_.state_ == NodesState_HoverConnection;
					consider_io |= element_.state_ == NodesState_HoverNode;

					// from nothing to hovered output
					if (consider_io)
					{
						element_.Reset(NodesState_HoverIO);
						element_.node_ = node.Get();
						element_.connection_ = connection.get();
						element_.position_ = node.position_ + connection->position_;
					}

					// we could start draging output now
					if (ImGui::IsMouseClicked(0) && element_.connection_ == connection.get())
					{
						element_.state_ = NodesState_DragingOutput;
					}

					consider_io = true;
				}
				else if (element_.state_ == NodesState_HoverIO && element_.connection_ == connection.get())
				{
					element_.Reset(); // we are not hovering this last hovered output anymore
				}

				////////////////////////////////////////////////////////////////////////////////

				ImColor color = ImColor(0.5f, 0.5f, 0.5f, 1.0f);

				if (connection->connections_ > 0)
				{
					drawList->AddCircleFilled(connection_pos, (output_name_size.y / 3.0f), ImColor(0.5f, 0.5f, 0.5f, 1.0f));
				}

				// currently we are dragin some input, check if there is a possibilty to connect here (this output)
				if (element_.state_ == NodesState_DragingInput || element_.state_ == NodesState_DragingInputValid)
				{
					// check is draging input are not from the same node
					if (element_.node_ != node.Get() && element_.connection_->type_ == connection->type_)
					{
						color = ImColor(0.0f, 1.0f, 0.0f, 1.0f);

						if (consider_io)
						{
							element_.state_ = NodesState_DragingInputValid;
							drawList->AddCircleFilled(connection_pos, (output_name_size.y / 3.0f), color);

							if (!ImGui::IsMouseDown(0))
							{
								element_.connection_->target_ = node.Get();
								element_.connection_->input_ = connection.get();
								element_.connection_->connections_ = 1;

								connection->connections_++;

								element_.Reset(NodesState_HoverIO);
								element_.node_ = node.Get();
								element_.connection_ = connection.get();
								element_.position_ = node_rect_min + connection->position_;
							}
						}
					}
				}

				consider_io |= element_.state_ == NodesState_HoverIO;
				consider_io |= element_.state_ == NodesState_DragingOutput;
				consider_io |= element_.state_ == NodesState_DragingOutputValid;
				consider_io &= element_.connection_ == connection.get();

				if (consider_io)
				{
					color = ImColor(0.0f, 1.0f, 0.0f, 1.0f);

					if (element_.state_ != NodesState_HoverIO)
					{
						drawList->AddCircleFilled(connection_pos, (output_name_size.y / 3.0f), color);
					}
				}

				drawList->AddCircle(connection_pos, (output_name_size.y / 3.0f), color, ((int)(6.0f * canvas_scale_) + 10), (1.5f * canvas_scale_));
			}
			
			////////////////////////////////////////////////////////////////////////////////		
		}

		////////////////////////////////////////////////////////////////////////////////

		if ((consider_select && consider_hover) || (node.id_ < 0))
		{
			drawList->AddRectFilled(node_rect_min, node_rect_max, ImColor(1.0f, 1.0f, 1.0f, 0.25f), corner, ImDrawCornerFlags_All);
		}

		ImGui::EndGroup();
		ImGui::PopID();
	}

	void Nodes::ProcessNodes()
	{
		////////////////////////////////////////////////////////////////////////////////
		
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

		ImGui::BeginChild("NodesScrollingRegion", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		////////////////////////////////////////////////////////////////////////////////

		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		{
			canvas_mouse_ = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
			canvas_position_ = ImGui::GetCursorScreenPos();
			canvas_size_ = ImGui::GetWindowSize();

			UpdateScroll();
		}
		
		////////////////////////////////////////////////////////////////////////////////

		{
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			ImU32 color = ImColor(0.5f, 0.5f, 0.5f, 0.1f);
			const float size = 64.0f * canvas_scale_;

			for (float x = fmodf(canvas_scroll_.x, size); x < canvas_size_.x; x += size)
			{
				draw_list->AddLine(ImVec2(x, 0.0f) + canvas_position_, ImVec2(x, canvas_size_.y) + canvas_position_, color);
			}

			for (float y = fmodf(canvas_scroll_.y, size); y < canvas_size_.y; y += size)
			{
				draw_list->AddLine(ImVec2(0.0f, y) + canvas_position_, ImVec2(canvas_size_.x, y) + canvas_position_, color);
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		ImVec2 offset = canvas_position_ + canvas_scroll_;

		UpdateState(offset);
		RenderLines(draw_list, offset);
		DisplayNodes(draw_list, offset);

		if (element_.state_ == NodesState_SelectingEmpty || element_.state_ == NodesState_SelectingValid || element_.state_ == NodesState_SelectingMore)
		{
			draw_list->AddRectFilled(element_.rect_.Min, element_.rect_.Max, ImColor(200.0f, 200.0f, 0.0f, 0.1f));
			draw_list->AddRect(element_.rect_.Min, element_.rect_.Max, ImColor(200.0f, 200.0f, 0.0f, 0.5f));
		}

		////////////////////////////////////////////////////////////////////////////////
		
		{
			ImGui::SetCursorScreenPos(canvas_position_);

			bool consider_menu = !ImGui::IsAnyItemHovered();
			consider_menu &= ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
			consider_menu &= element_.state_ == NodesState_Default || element_.state_ == NodesState_Selected;
			consider_menu &= ImGui::IsMouseReleased(1);		
			
			if (consider_menu)
			{
				ImGuiContext* context = ImGui::GetCurrentContext();

				if (context->IO.MouseDragMaxDistanceSqr[1] < 36.0f)
				{
					ImGui::OpenPopup("NodesContextMenu");
				}								
			}

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
			if (ImGui::BeginPopup("NodesContextMenu"))
			{
				element_.Reset(NodesState_Block);

				for (auto& node : nodes_types_)
				{
					if (ImGui::MenuItem(node.name_.c_str()))
					{					
						element_.Reset();
						element_.node_ = CreateNodeFromType((canvas_mouse_ - canvas_scroll_) / canvas_scale_, node);
					}
				}				
				ImGui::EndPopup();
			}
			ImGui::PopStyleVar();
		}

		////////////////////////////////////////////////////////////////////////////////

//		{
//			ImGui::SetCursorScreenPos(canvas_position_);
//		
//			switch (element_.state_)
//			{
//				case NodesState_Default: ImGui::Text("NodesState_Default"); break;
//				case NodesState_Block: ImGui::Text("NodesState_Block"); break;
//				case NodesState_HoverIO: ImGui::Text("NodesState_HoverIO"); break;
//				case NodesState_HoverConnection: ImGui::Text("NodesState_HoverConnection"); break;
//				case NodesState_HoverNode: ImGui::Text("NodesState_HoverNode"); break;
//				case NodesState_DragingInput: ImGui::Text("NodesState_DragingInput"); break;
//				case NodesState_DragingInputValid: ImGui::Text("NodesState_DragingInputValid"); break;
//				case NodesState_DragingOutput: ImGui::Text("NodesState_DragingOutput"); break;
//				case NodesState_DragingOutputValid: ImGui::Text("NodesState_DragingOutputValid"); break;
//				case NodesState_DragingConnection: ImGui::Text("NodesState_DragingConnection"); break;
//				case NodesState_DragingSelected: ImGui::Text("NodesState_DragingSelected"); break;
//				case NodesState_SelectingEmpty: ImGui::Text("NodesState_SelectingEmpty"); break;
//				case NodesState_SelectingValid: ImGui::Text("NodesState_SelectingValid"); break;
//				case NodesState_SelectingMore: ImGui::Text("NodesState_SelectingMore"); break;
//				case NodesState_Selected: ImGui::Text("NodesState_Selected"); break;
//				case NodesState_SelectedConnection: ImGui::Text("NodesState_SelectedConnection"); break;
//				default: ImGui::Text("UNKNOWN"); break;
//			}
//		
//			ImGui::Text("");
//		
//			ImGui::Text("Mouse: %.2f, %.2f", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
//			ImGui::Text("Mouse delta: %.2f, %.2f", ImGui::GetIO().MouseDelta.x, ImGui::GetIO().MouseDelta.y);
//			ImGui::Text("Offset: %.2f, %.2f", offset.x, offset.y);
//		
//			ImGui::Text("");
//		
//			ImGui::Text("Canvas_mouse: %.2f, %.2f", canvas_mouse_.x, canvas_mouse_.y);
//			ImGui::Text("Canvas_position: %.2f, %.2f", canvas_position_.x, canvas_position_.y);
//			ImGui::Text("Canvas_size: %.2f, %.2f", canvas_size_.x, canvas_size_.y);
//			ImGui::Text("Canvas_scroll: %.2f, %.2f", canvas_scroll_.x, canvas_scroll_.y);
//			ImGui::Text("Canvas_scale: %.2f", canvas_scale_);
//		}

		////////////////////////////////////////////////////////////////////////////////

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}
}