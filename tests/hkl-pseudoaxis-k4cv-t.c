/* This file is part of the hkl library.
 *
 * The hkl library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The hkl library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the hkl library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2003-2013 Synchrotron SOLEIL
 *                         L'Orme des Merisiers Saint-Aubin
 *                         BP 48 91192 GIF-sur-YVETTE CEDEX
 *
 * Authors: Picca Frédéric-Emmanuel <picca@synchrotron-soleil.fr>
 */
#include "hkl.h"
#include <tap/basic.h>
#include <tap/hkl-tap.h>

static void degenerated(void)
{
	int res = HKL_TRUE;
	HklEngineList *engines;
	HklEngine *engine;
	HklMode **mode;
	darray_mode *modes;
	const HklFactory *factory;
	HklGeometry *geometry;
	const HklGeometryList *geometries;
	HklDetector *detector;
	HklSample *sample;
	static double hkl[] = {0, 1, 0};
	HklParameterList *pseudo_axes;
	darray_parameter *parameters;

	factory = hkl_factory_get_by_name("K4CV");
	geometry = hkl_factory_create_new_geometry(factory);
	sample = hkl_sample_new("test");

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	hkl_detector_idx_set(detector, 1);

	engines = hkl_factory_create_new_engine_list(factory);
	hkl_engine_list_init(engines, geometry, detector, sample);
	geometries = hkl_engine_list_geometries(engines);

	engine = hkl_engine_list_get_by_name(engines, "hkl");
	modes = hkl_engine_modes(engine);
	pseudo_axes = hkl_engine_pseudo_axes(engine);

	darray_foreach(mode, *modes) {
		hkl_engine_select_mode(engine, *mode);
		parameters = hkl_mode_parameters(*mode);
		if(darray_size(*parameters))
			hkl_parameter_value_set(darray_item(*parameters, 0), 1, NULL);

		/* studdy this degenerated case */
		hkl_parameter_list_values_set(pseudo_axes, hkl, ARRAY_SIZE(hkl), NULL);
		if (hkl_engine_set(engine, NULL)){
			const darray_item *items = hkl_geometry_list_items_get(geometries);
			HklGeometryListItem **item;

			darray_foreach(item, *items){
				static double null[] = {0, 0, 0};

				hkl_parameter_list_values_set(pseudo_axes,
							      null, ARRAY_SIZE(null),
							      NULL);
				hkl_geometry_set(geometry,
						 hkl_geometry_list_item_geometry_get(*item));
				hkl_engine_get(engine, NULL);
				res &= check_pseudoaxes(engine, hkl, 3);
			}
		}
	}

	ok(res == HKL_TRUE, "degenerated");

	hkl_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geometry);
}

static void eulerians(void)
{
	int res = HKL_TRUE;
	HklEngineList *engines;
	HklEngine *engine;
	HklMode **mode;
	darray_mode *modes;
	const HklFactory *factory;
	HklGeometry *geometry;
	const HklGeometryList *geometries;
	HklDetector *detector;
	HklSample *sample;
	static double eulerians[] = {0., 90 * HKL_DEGTORAD, 0.};
	HklParameterList *pseudo_axes;

	factory = hkl_factory_get_by_name("K4CV");
	geometry = hkl_factory_create_new_geometry(factory);
	sample = hkl_sample_new("test");

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	hkl_detector_idx_set(detector, 1);

	engines = hkl_factory_create_new_engine_list(factory);
	hkl_engine_list_init(engines, geometry, detector, sample);
	geometries = hkl_engine_list_geometries(engines);

	engine = hkl_engine_list_get_by_name(engines, "eulerians");
	modes = hkl_engine_modes(engine);
	pseudo_axes = hkl_engine_pseudo_axes(engine);

	darray_foreach(mode, *modes){
		darray_parameter *parameters;

		hkl_engine_select_mode(engine, *mode);
		parameters = hkl_mode_parameters(*mode);
		if(darray_size(*parameters))
			hkl_parameter_value_set(darray_item(*parameters, 0), 1, NULL);

		/* studdy this degenerated case */
		hkl_parameter_list_values_set(pseudo_axes,
					      eulerians, ARRAY_SIZE(eulerians),
					      NULL);
		if (hkl_engine_set(engine, NULL)) {
			const darray_item *items = hkl_geometry_list_items_get(geometries);

			res &= darray_size(*items) == 2;

			/* first solution = 0, 90, 0 */
			hkl_geometry_set(geometry,
					 hkl_geometry_list_item_geometry_get(darray_item(*items, 1)));
			hkl_engine_get(engine, NULL);
			res &= check_pseudoaxes_v(engine, 0., 90 * HKL_DEGTORAD, 0.);

			/* second solution = -180, -90, 180 */
			hkl_geometry_set(geometry,
					 hkl_geometry_list_item_geometry_get(darray_item(*items, 0)));
			hkl_engine_get(engine, NULL);
			res &= check_pseudoaxes_v(engine, -180. * HKL_DEGTORAD, -90 * HKL_DEGTORAD, 180. * HKL_DEGTORAD);
		}
	}

	ok(res == HKL_TRUE, "eulerians");

	hkl_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geometry);
}

static void q(void)
{
	int res = HKL_TRUE;
	HklEngineList *engines;
	HklEngine *engine;
	HklMode **mode;
	darray_mode *modes;
	const HklFactory *factory;
	HklGeometry *geometry;
	const HklGeometryList *geometries;
	HklDetector *detector;
	HklSample *sample;

	factory = hkl_factory_get_by_name("K4CV");
	geometry = hkl_factory_create_new_geometry(factory);
	sample = hkl_sample_new("test");

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	hkl_detector_idx_set(detector, 1);

	engines = hkl_factory_create_new_engine_list(factory);
	hkl_engine_list_init(engines, geometry, detector, sample);
	geometries = hkl_engine_list_geometries(engines);

	engine = hkl_engine_list_get_by_name(engines, "q");
	modes = hkl_engine_modes(engine);

	/* the init part */
	hkl_geometry_set_values_unit_v(geometry, 30., 0., 0., 60.);
	hkl_engine_initialize(engine, NULL);

	darray_foreach(mode, *modes){
		double q;

		hkl_engine_select_mode(engine, *mode);
		for(q=-1.; q<1.; q += 0.1){
			hkl_engine_set_values_v(engine, q, NULL);
			if(hkl_engine_set(engine, NULL)){
				const darray_item *items = hkl_geometry_list_items_get(geometries);
				HklGeometryListItem **item;

				darray_foreach(item, *items){
					hkl_engine_set_values_v(engine, 0.);
					hkl_geometry_set(geometry,
							 hkl_geometry_list_item_geometry_get(*item));
					hkl_engine_get(engine, NULL);
					res &= check_pseudoaxes_v(engine, q);
				}
			}
		}
	}

	ok(res == HKL_TRUE, "q");

	hkl_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geometry);
}

int main(int argc, char** argv)
{
	plan(3);

	degenerated();
	eulerians();
	q();

	return 0;
}