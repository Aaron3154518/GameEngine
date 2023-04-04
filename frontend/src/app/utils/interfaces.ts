import { Observable, Subject } from 'rxjs';
import { newUUID } from './utils';
import { ParameterService } from '../services/parameter.service';

export type StringDict<T> = { [key: string]: T };

export function toDict<T>(list: T[], getKey: (t: T) => string): StringDict<T> {
  return list.reduce((dict: StringDict<T>, t: T) => {
    dict[getKey(t)] = t;
    return dict;
  }, {});
}

export interface Vertex {
  data?: any;
  set: Parameters;
  group: ParameterGroup;
  name: string;
  in: Edge[];
  out: Edge[];
}

export interface Edge {
  data?: any;
  callback: Callback;
  from: Vertex;
  to: Vertex;
}

// Parameter Group
export interface IParameterGroup {
  uuid: string;
  name: string;
  params: Set<string>;
}

type IParameterGroupOpt = {
  uuid?: string;
  name?: string;
  params?: Set<string>;
};

export class ParameterGroup implements IParameterGroup {
  readonly uuid: string;
  readonly name: string;
  params: Set<string>;

  private changeSubject: Subject<void> = new Subject();
  $changes: Observable<void> = this.changeSubject.asObservable();

  constructor({
    uuid = newUUID(),
    name = '',
    params = new Set<string>(),
  }: IParameterGroupOpt = {}) {
    this.uuid = uuid;
    this.name = name;
    this.params = params;
  }

  hasParam(param: string): boolean {
    return this.params.has(param);
  }

  addParam(param: string) {
    this.params.add(param);
    this.changeSubject.next();
  }

  removeParam(param: string) {
    this.params.delete(param);
    this.changeSubject.next();
  }
}

// Parameters
export interface IParameters {
  uuid: string;
  name: string;
  type: string;
  groups: Set<string>;
}

type IParametersOpt = {
  uuid?: string;
  name?: string;
  type?: string;
  groups?: Set<string> | string[];
};

export class Parameters implements IParameters {
  readonly uuid: string;
  name: string;
  type: string;
  group: ParameterGroup;
  groups: Set<string>;

  private changeSubject: Subject<void> = new Subject();
  $changes: Observable<void> = this.changeSubject.asObservable();

  constructor({
    uuid = newUUID(),
    name = '',
    type = '',
    groups = [],
  }: IParametersOpt = {}) {
    this.uuid = uuid;
    this.name = name;
    this.type = type;
    this.group = new ParameterGroup();
    this.groups = new Set<string>(groups);

    let al = 'ABCDEFGHIJ';
    [...Array(al.length).keys()].forEach((i: number) =>
      this.group.addParam(al.substring(0, i + 1))
    );
  }

  addParam(param: string) {
    this.group.addParam(param);
    this.changeSubject.next();
  }

  removeParam(param: string) {
    this.group.removeParam(param);
    this.changeSubject.next();
  }

  addGroup(id: string): void {
    this.groups.add(id);
    this.changeSubject.next();
  }

  removeGroup(id: string): void {
    this.groups.delete(id);
    this.changeSubject.next();
  }
}

// Callbacks
// TODO: duplicated accross guuid
// Parameters uuid (as string) : {parameter name : parameter alias}
export type CallbackParameters = StringDict<StringDict<string>>;
export interface CallbackParameter {
  name: string;
  alias: string;
}
export interface CallbackParameterList {
  set: Parameters;
  params: CallbackParameter[];
}
export interface CallbackReturn {
  uuid: string;
  name: string;
}

export interface ICallback {
  uuid: string;
  name: string;
  code: string;
  params: CallbackParameters;
  retParam?: CallbackReturn;
}

type ICallbackOpt = {
  uuid?: string;
  name?: string;
  code?: string;
  params?: CallbackParameters;
  retParam?: CallbackReturn;
};

export enum CodeType {
  Code = 0,
  Type = 1,
  Var = 2,
}

export class Callback implements ICallback {
  readonly uuid: string;
  name: string;
  code: string;
  params: CallbackParameters;
  retParam?: CallbackReturn;

  private changeSubject: Subject<void> = new Subject();
  $changes: Observable<void> = this.changeSubject.asObservable();

  constructor({
    uuid = newUUID(),
    name = '',
    code = '',
    params = {},
    retParam = undefined,
  }: ICallbackOpt = {}) {
    this.uuid = uuid;
    this.name = name;
    this.code = code;
    this.params = params;
    this.retParam = retParam;
  }

  addParameter(uuid: string, name: string) {
    let params: StringDict<string> = this.params[uuid];
    if (!params) {
      params = this.params[uuid] = {};
    }
    params[name] = name;
    this.changeSubject.next();
  }

  removeParameter(uuid: string, name: string) {
    if (!this.params[uuid]) {
      return;
    }
    delete this.params[uuid][name];
    this.changeSubject.next();
  }

  getParameterAlias(uuid: string, name: string): string {
    return this.params[uuid] ? this.params[uuid][name] : '';
  }

  setParameterAlias(uuid: string, name: string, alias: string) {
    let d: StringDict<string> = this.params[uuid];
    if (d && name in d) {
      d[name] = alias;
    }
  }

  setParameterAliases(
    _plists: CallbackParameterList | CallbackParameterList[]
  ) {
    let plists: CallbackParameterList[] = Array.isArray(_plists)
      ? _plists
      : [_plists];
    plists.forEach((plist: CallbackParameterList) => {
      let d: StringDict<string> = this.params[plist.set.uuid];
      if (d) {
        plist.params.forEach((p: { name: string; alias: string }) => {
          if (p.name in d) {
            d[p.name] = p.alias;
          }
        });
      }
    });
  }

  static getParametersFromList(data: [string, string[]][]): CallbackParameters {
    return data.reduce(
      (params: CallbackParameters, [uuid, names]: [string, string[]]) => {
        params[uuid] = {};
        names.forEach((s: string) => (params[uuid][s] = s));
        return params;
      },
      {}
    );
  }
}
