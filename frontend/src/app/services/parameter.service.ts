import { Injectable } from '@angular/core';
import { BehaviorSubject, Observable } from 'rxjs';
import {
  Callback,
  ParameterGroup,
  Parameters,
  StringDict,
  toDict,
} from '../utils/interfaces';

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
    new ParameterGroup({
      name: 'Fruits',
      params: new Set([
        'Banana',
        'Apple',
        'Orange',
        'Lemon',
        'Mango',
        'Papaya',
      ]),
    }),
  ];
  private _paramGroupDict: StringDict<ParameterGroup> = toDict(
    this._paramGroups,
    (pg: ParameterGroup) => pg.uuid
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
      groups: new Set(this._paramGroups.map((g: ParameterGroup) => g.uuid)),
    }),
    new Parameters({
      name: 'GenVals',
      type: 'Number',
      groups: new Set([this._paramGroups[1].uuid]),
    }),
    new Parameters({
      name: 'GenNames',
      type: 'std::string',
      groups: new Set([this._paramGroups[1].uuid]),
    }),
  ];
  private _paramSetDict: StringDict<Parameters> = toDict(
    this._paramSets,
    (ps: Parameters) => ps.uuid
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
    (cb: Callback) => cb.uuid
  );
  private callbacksChanged: BehaviorSubject<null> = new BehaviorSubject<null>(
    null
  );
  $callbacksChanged: Observable<null> = this.callbacksChanged.asObservable();

  constructor() {}

  get paramGroups(): Readonly<ParameterGroup[]> {
    return this._paramGroups;
  }

  get paramGroupDict(): Readonly<StringDict<ParameterGroup>> {
    return this._paramGroupDict;
  }

  getParamGroup(idx: number): ParameterGroup | undefined;
  getParamGroup(uuid: string): ParameterGroup | undefined;
  getParamGroup(idx: number | string): ParameterGroup | undefined {
    if (typeof idx === 'number') {
      return this.paramGroups[idx];
    }

    let group: ParameterGroup = this.paramGroupDict[idx];
    if (!group) {
      let i: number = this.paramSets.findIndex(
        (set: Parameters) => idx === set.group.uuid
      );
      if (i !== -1) {
        group = this.paramSets[i].group;
      }
    }
    return group;
  }

  removeParamGroup(idx: number): void;
  removeParamGroup(uuid: string): void;
  removeParamGroup(idx: number | string): void {
    let i: number =
      typeof idx === 'number'
        ? idx
        : this.paramGroups.findIndex((g: ParameterGroup) => g.uuid === idx);
    if (i !== -1) {
      let uuid: string = this._paramGroups.splice(i, 1)[0].uuid;
      delete this._paramGroupDict[uuid];
      this.paramSets.forEach((set: Parameters) => set.removeGroup(uuid));
      // TODO: invalidate callbacks
    }
  }

  newParamGroup(group: ParameterGroup) {
    this._paramGroups.push(group);
    this._paramGroupDict[group.uuid] = group;
    this.paramGroupsChanged.next(null);
  }

  get paramSets(): Readonly<Parameters[]> {
    return this._paramSets;
  }

  get paramSetDict(): Readonly<StringDict<Parameters>> {
    return this._paramSetDict;
  }

  getParamSet(idx: number): Parameters | undefined;
  getParamSet(uuid: string): Parameters | undefined;
  getParamSet(idx: number | string): Parameters | undefined {
    if (typeof idx === 'number') {
      return this.paramSets[idx];
    }
    return this.paramSetDict[idx];
  }

  removeParamSet(idx: number): void;
  removeParamSet(uuid: string): void;
  removeParamSet(idx: number | string): void {
    let i: number =
      typeof idx === 'number'
        ? idx
        : this.paramSets.findIndex((s: Parameters) => s.uuid === idx);
    if (i !== -1) {
      delete this._paramSetDict[this._paramSets.splice(i, 1)[0].uuid];
      // TODO: invalidate callbacks
    }
  }

  newParamSet(set: Parameters) {
    this._paramSets.push(set);
    this._paramSetDict[set.uuid] = set;
    this.paramSetsChanged.next(null);
  }

  get callbacks(): Readonly<Callback[]> {
    return this._callbacks;
  }

  get callbackDict(): Readonly<StringDict<Callback>> {
    return this._callbackDict;
  }

  getCallback(idx: number): Callback | undefined;
  getCallback(uuid: string): Callback | undefined;
  getCallback(idx: number | string): Callback | undefined {
    if (typeof idx === 'number') {
      return this.callbacks[idx];
    }
    return this.callbackDict[idx];
  }

  removeCallback(idx: number): void;
  removeCallback(uuid: string): void;
  removeCallback(idx: number | string): void {
    let i: number =
      typeof idx === 'number'
        ? idx
        : this.callbacks.findIndex((cb: Callback) => cb.uuid === idx);
    if (i !== -1) {
      delete this._callbackDict[this._callbacks.splice(i, 1)[0].uuid];
    }
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
