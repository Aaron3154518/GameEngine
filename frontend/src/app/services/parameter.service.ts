import { Injectable } from '@angular/core';
import { BehaviorSubject, Observable } from 'rxjs';

const fs = (window as any).require('fs');

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
    this.paramLists.value[0].params[0].callbacks['Test'] = 'doThing();';
    this.paramLists.next(this.paramLists.value);
  }

  codegen() {
    fs.writeFileSync('files/params.cpp', 'Hello World');
  }
}
