// Prototype of standalone node graph editor for ImGui
// Thread: https://github.com/ocornut/imgui/issues/306
//
// This is based on code by:
// @emoon https://gist.github.com/emoon/b8ff4b4ce4f1b43e79f2
// @ocornut https://gist.github.com/ocornut/7e9b3ec566a333d725d4
// @flix01 https://github.com/Flix01/imgui/blob/b248df2df98af13d4b7dbb70c92430afc47a038a/addons/imguinodegrapheditor/imguinodegrapheditor.cpp#L432

#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"

#include <memory>
#include <string>
#include <vector>
#include <algorithm>

namespace ImGui
{
	enum ConnectionType : uint32_t
	{
		ConnectionType_None = 0,
		ConnectionType_Vec3,
		ConnectionType_Float,
		ConnectionType_Int,
	};

	////////////////////////////////////////////////////////////////////////////////

	struct NodeType
	{
		std::string name_;
		std::vector<std::pair<std::string, ConnectionType>> inputs_;
		std::vector<std::pair<std::string, ConnectionType>> outputs_;
	};

	template<int n>
	struct BezierWeights
	{
		BezierWeights() : x_(), y_(), z_(), w_()
		{
			for (int i = 1; i <= n; ++i)
			{
				float t = (float)i / (float)(n + 1);
				float u = 1.0f - t;

				x_[i - 1] = u * u * u;
				y_[i - 1] = 3 * u * u * t;
				z_[i - 1] = 3 * u * t * t;
				w_[i - 1] = t * t * t;
			}
		}

		float x_[n];
		float y_[n];
		float z_[n];
		float w_[n];
	};

	static auto bezier_weights_ = BezierWeights<16>();

	////////////////////////////////////////////////////////////////////////////////

	static const std::vector<ImGui::NodeType> nodes_types_=
	{
		////////////////////////////////////////////////////////////////////////////////
		{
			{ std::string("Test") },

			{
				{ std::string("FloatTEST"), ImGui::ConnectionType_Float },
				{ std::string("Int"), ImGui::ConnectionType_Int }
			},

			{
				{ std::string("Float"), ImGui::ConnectionType_Float }
			}
		},
		////////////////////////////////////////////////////////////////////////////////
		{
			{ std::string("BigInput") },

			{
				{ std::string("Float1"), ImGui::ConnectionType_Float },
				{ std::string("Float2"), ImGui::ConnectionType_Float },
				{ std::string("Int1"), ImGui::ConnectionType_Int },
				{ std::string("Int2"), ImGui::ConnectionType_Int },
				{ {}, ImGui::ConnectionType_None },
				{ std::string("Float3"), ImGui::ConnectionType_Float },
				{ std::string("Float4"), ImGui::ConnectionType_Float },
				{ std::string("Float5"), ImGui::ConnectionType_Float }
			},

			{
				{ std::string("Float1"), ImGui::ConnectionType_Float },
				{ std::string("Int1"), ImGui::ConnectionType_Int },
				{ {}, ImGui::ConnectionType_None },
				{ std::string("Vec1"), ImGui::ConnectionType_Vec3 }
			}
		},
		////////////////////////////////////////////////////////////////////////////////
		{
			{ std::string("BigOutput") },

			{
				{ std::string("VecTEST"), ImGui::ConnectionType_Vec3 },
				{ {}, ImGui::ConnectionType_None },
				{ std::string("Int"), ImGui::ConnectionType_Int },
				{ std::string("Float"), ImGui::ConnectionType_Float }
			},

			{
				{ std::string("FloatTEST"), ImGui::ConnectionType_Float },
				{ std::string("Float"), ImGui::ConnectionType_Float },
				{ {}, ImGui::ConnectionType_None },
				{ std::string("Int1"), ImGui::ConnectionType_Int },
				{ std::string("Int2"), ImGui::ConnectionType_Int },
				{ {}, ImGui::ConnectionType_None },
				{ std::string("VecABC"), ImGui::ConnectionType_Vec3 },
				{ std::string("VecXYZ"), ImGui::ConnectionType_Vec3 }
			}
		}
		////////////////////////////////////////////////////////////////////////////////
	};

	////////////////////////////////////////////////////////////////////////////////

	class Nodes final
	{
	private:	
		////////////////////////////////////////////////////////////////////////////////

		struct Node;

		struct Connection
		{
			ImVec2 position_;
			std::string name_;
			ConnectionType type_;

			Node* target_;
			Connection* input_;
			uint32_t connections_;

			Connection()
			{
				position_ = ImVec2(0.0f, 0.0f);

				type_ = ConnectionType_None;

				target_ = nullptr;
				input_ = nullptr;
				connections_ = 0;
			}

			Connection* Get()
			{
				return this;
			}
		};

		////////////////////////////////////////////////////////////////////////////////

		enum NodeStateFlag : int32_t
		{
			NodeStateFlag_Default = 1,
		};

		struct Node
		{
			int32_t id_; // 0 = empty, positive/negative = not selected/selected
			int32_t state_;

			ImVec2 position_;
			ImVec2 size_;

			float collapsed_height;
			float full_height;

			std::string name_;
			std::vector<std::unique_ptr<Connection>> inputs_;
			std::vector<std::unique_ptr<Connection>> outputs_;

			Node()
			{
				id_ = 0;
				state_ = NodeStateFlag_Default;

				position_ = ImVec2(0.0f, 0.0f);
				size_ = ImVec2(0.0f, 0.0f);

				collapsed_height = 0.0f;
				full_height = 0.0f;
			}

			Node* Get()
			{
				return this;
			}
		};

		////////////////////////////////////////////////////////////////////////////////

		enum NodesState : uint32_t
		{
			NodesState_Default = 0,
			NodesState_Block, // helper: just block all states till next update (frame)
			NodesState_HoverIO,
			NodesState_HoverConnection,
			NodesState_HoverNode,
			NodesState_DragingInput,
			NodesState_DragingInputValid,
			NodesState_DragingOutput,
			NodesState_DragingOutputValid,
			NodesState_DragingConnection,
			NodesState_DragingSelected,
			NodesState_SelectingEmpty,
			NodesState_SelectingValid,
			NodesState_SelectingMore,
			NodesState_Selected,
			NodesState_SelectedConnection
		};

		struct NodesElement
		{
			NodesState state_;

			ImVec2 position_;
			ImRect rect_;

			Node* node_;
			Connection* connection_;

			void Reset(NodesState state = NodesState_Default)
			{
				state_ = state;

				position_ = ImVec2(0.0f, 0.0f);
				rect_ = ImRect(0.0f, 0.0f, 0.0f, 0.0f);

				node_ = nullptr;
				connection_ = nullptr;
			}
		};

		////////////////////////////////////////////////////////////////////////////////

		std::vector<std::unique_ptr<Node>> nodes_;

		int32_t id_;
		NodesElement element_;
		
		ImVec2 canvas_mouse_;
		ImVec2 canvas_position_;
		ImVec2 canvas_size_;
		ImVec2 canvas_scroll_;
		
		float canvas_scale_;

		////////////////////////////////////////////////////////////////////////////////

		float ImVec2Dot(const ImVec2& S1, const ImVec2& S2)
		{
			return (S1.x * S2.x + S1.y * S2.y);
		}

		float GetSquaredDistancePointSegment(const ImVec2& P, const ImVec2& S1, const ImVec2& S2)
		{
			const float l2 = (S1.x - S2.x) * (S1.x - S2.x) + (S1.y - S2.y) * (S1.y - S2.y);

			if (l2 < 1.0f)
			{
				return (P.x - S2.x) * (P.x - S2.x) + (P.y - S2.y) * (P.y - S2.y);
			}

			ImVec2 PS1(P.x - S1.x, P.y - S1.y);
			ImVec2 T(S2.x - S1.x, S2.y - S2.y);
			
			const float tf = ImVec2Dot(PS1, T) / l2;
			const float minTf = 1.0f < tf ? 1.0f : tf;
			const float t = 0.0f > minTf ? 0.0f : minTf;
			
			T.x = S1.x + T.x * t;
			T.y = S1.y + T.y * t;

			return (P.x - T.x) * (P.x - T.x) + (P.y - T.y) * (P.y - T.y);
		}

		float GetSquaredDistanceToBezierCurve(const ImVec2& point, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4)
		{
			float minSquaredDistance = FLT_MAX;
			float tmp;

			ImVec2 L = p1;
			ImVec2 temp;

			for (int i = 1; i < 16 - 1; ++i)
			{
				const ImVec4& W = ImVec4(bezier_weights_.x_[i], bezier_weights_.y_[i], bezier_weights_.z_[i], bezier_weights_.w_[i]);

				temp.x = W.x * p1.x + W.y * p2.x + W.z * p3.x + W.w * p4.x;
				temp.y = W.x * p1.y + W.y * p2.y + W.z * p3.y + W.w * p4.y;

				tmp = GetSquaredDistancePointSegment(point, L, temp);

				if (minSquaredDistance > tmp)
				{
					minSquaredDistance = tmp;
				}

				L = temp;
			}

			tmp = GetSquaredDistancePointSegment(point, L, p4);

			if (minSquaredDistance > tmp)
			{
				minSquaredDistance = tmp;
			}

			return minSquaredDistance;
		}

		////////////////////////////////////////////////////////////////////////////////

		bool IsConnectorHovered(ImVec2 connection, float radius)
		{

			ImVec2 delta = ImGui::GetIO().MousePos - connection;

			return ((delta.x * delta.x) + (delta.y * delta.y)) < (radius * radius);
		}

		Node* GetHoverNode(ImVec2 offset, ImVec2 pos);
		void DisplayNode(ImDrawList* drawList, ImVec2 offset, Node& node);

		////////////////////////////////////////////////////////////////////////////////
		
		void UpdateScroll();
		void UpdateState(ImVec2 offset);
		void RenderLines(ImDrawList* draw_list, ImVec2 offset);
		void DisplayNodes(ImDrawList* drawList, ImVec2 offset);

		////////////////////////////////////////////////////////////////////////////////

		Nodes::Node*  CreateNodeFromType(ImVec2 pos, const NodeType& type);

	public:
		explicit Nodes();
		~Nodes();

		void ProcessNodes();
	};
}