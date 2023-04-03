import { Injectable } from '@angular/core';
import { ParameterService } from './parameter.service';
import { ParameterGroup, Parameters } from '../utils/interfaces';

@Injectable({
  providedIn: 'root',
})
export class ParameterDragService {
  readonly format: string = 'applicaton/json';

  constructor(private parameterService: ParameterService) {}

  onDragStart(event: DragEvent, data: ParameterDragService.DragData) {
    event.dataTransfer?.setData(this.format, JSON.stringify(data));
  }

  dropInTrash(event: DragEvent) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (data.type === ParameterDragService.DataType.Group) {
        this.parameterService.removeParamGroup(data.value);
      } else if (
        data.type === ParameterDragService.DataType.Var &&
        data.srcUUID
      ) {
        this.parameterService
          .getParamGroup(data.srcUUID)
          ?.removeParam(data.value);
      }
    }
  }

  dropOnGroup(event: DragEvent, group: ParameterGroup) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (data.type === ParameterDragService.DataType.Var) {
        group.addParam(data.value);
      }
    }
  }

  dropOnSet(event: DragEvent, set: Parameters) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (data.type === ParameterDragService.DataType.Group) {
        set.addGroup(data.value);
      } else if (data.type === ParameterDragService.DataType.Var) {
        set.addParam(data.value);
      }
    }
  }

  onDragOver(event: DragEvent) {
    event.stopPropagation();
    event.preventDefault();
  }
}

export namespace ParameterDragService {
  export enum DataType {
    Var = 0,
    Group = 1,
    Set = 2,
    Callback = 3,
  }

  export interface DragData {
    type: DataType;
    value: string;
    srcUUID?: string;
  }
}
