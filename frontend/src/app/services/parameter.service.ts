import { Injectable } from '@angular/core';
import { BehaviorSubject, Observable } from 'rxjs';

const fs: any = undefined; //(window as any).require('fs');

export interface Param {
  name: string;
  callbacks: { [key: string]: string };
}

export function newParam(name: string = ''): Param {
  return {
    name: name,
    callbacks: {},
  };
}

export class ParamInfo {
  constructor(readonly list: ParamList, readonly i: number) {}

  get name(): string {
    return `${this.list.name}_${this.list.params[this.i].name}`;
  }

  get type(): string {
    return this.list.type;
  }

  get param(): Param {
    return this.list.params[this.i];
  }
}

export interface ParamList {
  type: string;
  name: string;
  params: Param[];
}

export function newParamList(
  type: string = '',
  name: string = '',
  paramNames: string[] = []
): ParamList {
  return {
    type: type,
    name: name,
    params: paramNames.map((n: string) => newParam(n)),
  };
}

@Injectable({
  providedIn: 'root',
})
export class ParameterService {
  private paramLists: BehaviorSubject<ParamList[]> = new BehaviorSubject<
    ParamList[]
  >([
    newParamList('Rect', 'Pos', ['Wizard', 'Crystal', 'Catalyst']),
    newParamList(
      'Number',
      'Gens',
      [...Array(5).keys()].map((i) => `T${i}`)
    ),
  ]);
  $paramLists: Observable<ParamList[]> = this.paramLists.asObservable();

  constructor() {
    this.paramLists.value[0].params[0].callbacks['Test'] =
      'std::cerr << Pos_Wizard << std::endl;';
    this.paramLists.value[1].params[1].callbacks['Inc'] =
      'std::cerr << (Gens_T1 + 1) << std::endl;';
    this.paramLists.next(this.paramLists.value);
  }

  codegen() {
    if (!fs) {
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
    this.paramLists.value.forEach((pl: ParamList, j: number) => {
      let enum_name: string = pl.name + 'Enum';
      let namespace: string = pl.name + 'Namespace';
      // Create enum in namespace to avoid enum name conflicts
      // Declare parameter set
      fs.appendFileSync(
        file,
        `namespace ${namespace}{ enum ${enum_name} : Observables::EnumT {${pl.params
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
    this.paramLists.value.forEach((pl: ParamList) => {
      let enum_name: string = pl.name + 'Enum';
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
    fs.closeSync(file);
  }
}
