#pragma once
#include "nav_area.h"
#include "../../utilities/micropather.h"

namespace nav_mesh {
	class nav_file : public micropather::Graph {
	public:
		nav_file( ) { }
		nav_file( std::string_view nav_mesh_file );
		
		void load( std::string_view nav_mesh_file );

		std::vector< Vector > find_path( Vector from, Vector to );
		std::vector< nav_area* > find_path( int fromId, int toId );
		std::vector< nav_area > m_areas = { };

		nav_area* GetNearestArea(Vector vPosition);
		std::string GetNameOfPlace(uint16_t placeIdx);

		//MicroPather implementation
		virtual float LeastCostEstimate( void* start, void* end ) {
			auto& start_area = get_area_by_id( std::uint32_t( start ) );
			auto& end_area = get_area_by_id( std::uint32_t( end ) );
			auto distance = start_area.get_center( ) - end_area.get_center( );

			return std::sqrtf( distance.x * distance.x + distance.y * distance.y + distance.z * distance.z );
		}

		virtual void AdjacentCost( void* state, micropather::MPVector< micropather::StateCost >* adjacent ) {
			auto& area = get_area_by_id( std::uint32_t( state ) );
			auto& area_connections = area.get_connections( );

			for ( auto& connection : area_connections ) {
				auto& connection_area = get_area_by_id( connection.id );
				auto distance = connection_area.get_center( ) - area.get_center( );

				micropather::StateCost cost = { reinterpret_cast< void* >( connection_area.get_id( ) ), 
					std::sqrtf( distance.x * distance.x + distance.y * distance.y + distance.z * distance.z ) };
				
				adjacent->push_back( cost );
			}
		}

		virtual void PrintStateInfo( void* state ) { }

	private:
		nav_area& get_area_by_id( std::uint32_t id );
		nav_area& get_area_by_position( Vector position );

		std::unique_ptr< micropather::MicroPather > m_pather = nullptr;

		std::uint8_t m_is_analyzed = 0,
			m_has_unnamed_areas = 0;

		std::uint16_t m_place_count = 0; 

		std::uint32_t m_magic = 0xFEEDFACE, 
			m_version = 0,
			m_sub_version = 0,
			m_source_bsp_size = 0,
			m_area_count = 0;

		nav_buffer m_buffer = { };
		std::vector< std::string > m_places = { };
	};
}