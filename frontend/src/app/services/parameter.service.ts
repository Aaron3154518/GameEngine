import { Injectable } from '@angular/core';
import { BehaviorSubject, Observable, Subject } from 'rxjs';
import {
  Callback,
  ParameterGroup,
  Parameters,
  StringDict,
  toDict,
} from '../utils/interfaces';
import { parse, stringify } from 'uuid';
import { UUID } from '../utils/utils';

const fs: any = undefined; //(window as any).require('fs');

@Injectable({
  providedIn: 'root',
})
export class ParameterService {
  private _paramGroups: ParameterGroup[] = [
    new ParameterGroup({
      name: 'Wizards',
      params: new Set(['Wizard', 'Crystal', 'Catalyst']),
    }),
    new ParameterGroup({
      name: 'Generators',
      params: new Set([...Array(5).keys()].map((i) => `T${i}`)),
    }),
  ];
  private _paramGroupDict: StringDict<ParameterGroup> = toDict(
    this._paramGroups,
    (pg: ParameterGroup) => stringify(pg.uuid)
  );
  private paramGroupsChanged: BehaviorSubject<null> = new BehaviorSubject<null>(
    null
  );
  $paramGroupsChanged: Observable<null> =
    this.paramGroupsChanged.asObservable();

  private _paramSets: Parameters[] = [
    new Parameters({
      name: 'Pos',
      type: 'Rect',
      groups: new Set([this._paramGroups[0].uuid]),
    }),
    new Parameters({
      name: 'Gens',
      type: 'Number',
      groups: new Set([this._paramGroups[1].uuid]),
    }),
    new Parameters({
      name: 'Gens',
      type: 'std::string',
      groups: new Set([this._paramGroups[1].uuid]),
    }),
  ];
  private _paramSetDict: StringDict<Parameters> = toDict(
    this._paramSets,
    (ps: Parameters) => stringify(ps.uuid)
  );
  private paramSetsChanged: BehaviorSubject<null> = new BehaviorSubject<null>(
    null
  );
  $paramSetsChanged: Observable<null> = this.paramSetsChanged.asObservable();

  private _callbacks: Callback[] = [
    new Callback({
      name: 'Test',
      code: 'std::cerr << Wizard << std::endl;',
      params: Callback.getParametersFromList([
        [this._paramSets[0].uuid, ['Wizard']],
      ]),
    }),
    new Callback({
      name: 'Inc',
      code: 'std::cerr << (T1 + T2 + T3) << std::endl;',
      params: Callback.getParametersFromList([
        [this._paramSets[1].uuid, ['T1']],
        [this._paramSets[2].uuid, ['T2', 'T3']],
      ]),
    }),
  ];
  private _callbackDict: StringDict<Callback> = toDict(
    this._callbacks,
    (cb: Callback) => stringify(cb.uuid)
  );
  private callbacksChanged: BehaviorSubject<null> = new BehaviorSubject<null>(
    null
  );
  $callbacksChanged: Observable<null> = this.callbacksChanged.asObservable();

  constructor() {}

  get paramGroups(): Readonly<ParameterGroup[]> {
    return this._paramGroups;
  }

  getParamGroup(i: number): ParameterGroup;
  getParamGroup(uuid: UUID): ParameterGroup;
  getParamGroup(uuid: string): ParameterGroup;
  getParamGroup(idx: number | UUID | string): ParameterGroup {
    if (typeof idx === 'number') {
      return this._paramGroups[idx];
    }

    let id: string = typeof idx !== 'string' ? stringify(idx) : idx;
    let uuid: UUID = typeof idx === 'string' ? parse(idx) : idx;
    let group: ParameterGroup = this._paramGroupDict[id];
    if (!group) {
      let i: number = this._paramSets.findIndex(
        (set: Parameters) => uuid === set.group.uuid
      );
      if (i !== -1) {
        group = this._paramSets[i].group;
      }
    }
    return group;
  }

  newParamGroup(group: ParameterGroup) {
    this._paramGroups.push(group);
    this._paramGroupDict[stringify(group.uuid)] = group;
    this.paramGroupsChanged.next(null);
  }

  get paramSets(): Readonly<Parameters[]> {
    return this._paramSets;
  }

  getParamSet(i: number): Parameters;
  getParamSet(uuid: UUID): Parameters;
  getParamSet(uuid: string): Parameters;
  getParamSet(idx: number | UUID | string): Parameters {
    if (typeof idx === 'number') {
      return this._paramSets[idx];
    }
    return this._paramSetDict[typeof idx === 'string' ? idx : stringify(idx)];
  }

  newParamSet(set: Parameters) {
    this._paramSets.push(set);
    this._paramSetDict[stringify(set.uuid)] = set;
    this.paramSetsChanged.next(null);
  }

  get callbacks(): Readonly<Callback[]> {
    return this._callbacks;
  }

  getCallback(i: number): Callback;
  getCallback(uuid: UUID): Callback;
  getCallback(uuid: string): Callback;
  getCallback(idx: number | UUID | string): Callback {
    if (typeof idx === 'number') {
      return this._callbacks[idx];
    }
    return this._callbackDict[typeof idx === 'string' ? idx : stringify(idx)];
  }

  codegen() {
    /*if (!fs) {
      return;
    }

    let file: number = fs.openSync('files/code.cpp', 'w');
    // Imports
    let imports: string[] = [
      'Observables/Observables.h',
      'Entities/Entity.h',
      'Utils/Number.h',
      'Utils/Rect.h',
    ];
    fs.appendFileSync(
      file,
      imports.map((i: string) => `#include <${i}>\n`).join('')
    );
    // Parameters
    this.parameters.forEach((pl: Parameters, j: number) => {
      let enum_name: string = pl.name + 'ParameterGroup';
      let namespace: string = pl.name + 'Namespace';
      // Create enum in namespace to avoid enum name conflicts
      // Declare parameter set
      fs.appendFileSync(
        file,
        `namespace ${namespace}{ enum ${enum_name} : Observables::ParameterGroupT {${pl.params
          .map((p: Param, i: number) => (i === 0 ? `${p.name}=0` : p.name))
          .join(',')}};}ROOT_NODE(${pl.name},${
          pl.type
        },${namespace}::${enum_name});`
      );
    });
    // Entity to manage subscriptions
    fs.appendFileSync(
      file,
      'class Dummy:public Entities::Entity{private:void init() {'
    );
    this.parameters.forEach((pl: Parameters) => {
      let enum_name: string = pl.name + 'ParameterGroup';
      let namespace: string = pl.name + 'Namespace';
      pl.params.forEach((p: Param) =>
        Object.values(p.callbacks).forEach((cb: string) =>
          fs.appendFileSync(
            file,
            'Observables::subscribe(' +
              `this,[](const ${pl.type}& ${pl.name}_${p.name}) {${cb}},` +
              `${pl.name}(${namespace}::${enum_name}::${p.name}));`
          )
        )
      );
    });
    fs.appendFileSync(file, `}};`);
    fs.closeSync(file);*/
  }
}
